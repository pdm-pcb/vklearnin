#include "vklearnin/common.hpp"
#include "vklearnin/RenderLoop.hpp"

#include "vklearnin/Instance.hpp"
#include "vklearnin/CommandStructures/CommandQueues.hpp"
#include "vklearnin/Swapchain.hpp"
#include "vklearnin/Pipeline.hpp"
#include "vklearnin/Framebuffers.hpp"
#include "vklearnin/Buffers/BufferObject.hpp"
#include "vklearnin/Buffers/UniformBufferObject.hpp"
#include "vklearnin/DescriptorSet.hpp"

#if defined(__linux__)
    #include "vklearnin/Platform/X11/X11Window.hpp"
#elif defined(_WIN32)
    #include "vklearnin/Win32Window.hpp"
#endif

// =============================================================================
bool RenderLoop::run(const Instance &instance, Swapchain &swapchain,
                     UniformBufferObject &ubo, Pipeline &pipeline,
                     DescriptorSet &descriptor_set, Framebuffers &framebuffers,
                     const BufferObject<Index> &index_buffer,
                     const std::vector<::VkBuffer> &vertex_buffers,
                     const std::vector<::VkDeviceSize> &vertex_buffer_offsets)
{
    CONSOLE_INFO("");

    ::VkResult result = ::VK_RESULT_MAX_ENUM;
    uint32_t frame_index = 0u;
    uint32_t image_index = 0u;

    while(_window.message_loop() == true) {
        // flip between zero and one, without a mod operation
        // courtesy paxdiablo: https://stackoverflow.com/a/4084058/1464937
        frame_index = 1 - frame_index;

        // wait your turn
        ::vkWaitForFences(_device, 1u, &_display_fences[frame_index], VK_TRUE,
                          UI64MAX);

        // grab the next swapchain image and check it...
        result = instance._AcquireNextImageKHR(
            _device,
            swapchain.swapchain(),
            UI64MAX,
            _image_available_sems[frame_index],
            nullptr,
            &image_index
        );

        // if we need to resize everything, let's do it
        if(result == ::VK_ERROR_OUT_OF_DATE_KHR) {
            _image_resized(instance, swapchain, pipeline, framebuffers);
            continue;   // be sure to continue so eveything updates
        }

        // now send the fresh data to the UBOs. Im curious: how much does order
        // matter here? Should this be done after the fences are reset? Is
        // that even relevant? Guess I'll have to read the spec. =)
        _update_ubo(ubo, swapchain, frame_index);

        // something tells me this could go outside the loop, as it never
        // changes, but it's fine for now
        ::VkCommandBufferBeginInfo buffer_info { };
        buffer_info.sType = ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // clear out what needs clearing
        ::vkResetFences(_device, 1u, &_display_fences[frame_index]);
        _queues.reset_command_buffer(
            image_index,
            static_cast<::VkCommandBufferResetFlagBits>(0u)
        );

        auto command_buffer = _queues.command_buffer(image_index);
        result = ::vkBeginCommandBuffer(command_buffer, &buffer_info);

        if(result != ::VK_SUCCESS) {
            CONSOLE_ERROR("Unable to begin command buffer recording.");
            return false;
        }

        // initial setup for the pass
        ::VkClearValue clear_values[] = {{
            .color = { 0.1f, 0.1f, 0.1f, 1.0f }
        }};

        ::VkRenderPassBeginInfo pass_info { };
        pass_info.sType = ::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        pass_info.renderPass = pipeline.renderpass();
        pass_info.framebuffer = framebuffers.buffer(image_index);

        auto [width, height] = swapchain.extent();
        auto [x, y]          = swapchain.offset();
        pass_info.renderArea.extent = { width, height };
        pass_info.renderArea.offset = { x, y };
        pass_info.clearValueCount =
            static_cast<uint32_t>(std::size(clear_values));
        pass_info.pClearValues = clear_values;

        // go time!
        ::vkCmdBeginRenderPass(command_buffer, &pass_info,
                               ::VK_SUBPASS_CONTENTS_INLINE);

            // bind the pipeline so everything's current
            ::vkCmdBindPipeline(
                command_buffer,
                ::VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipeline.pipeline()
            );

            // update the dynamic traits of the pipeline
            ::vkCmdSetViewport(command_buffer, 0u, 1u, &pipeline.viewport());
            ::vkCmdSetScissor(command_buffer,0u, 1u, &pipeline.scissor());

            // time for some host-side vertex data!
            ::vkCmdBindVertexBuffers(
                command_buffer,
                0u,
                vertex_buffers.size(),
                vertex_buffers.data(),
                vertex_buffer_offsets.data()
            );
            // and indices while we're at it
            ::vkCmdBindIndexBuffer(
                command_buffer,
                index_buffer.handle(),
                0u,
                IndexType
            );

            // time for some descriptor sets
            ::vkCmdBindDescriptorSets(
                command_buffer,
                ::VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipeline.layout(),
                0u, 1u,
                descriptor_set.sets().data(),
                0u, nullptr
            );

            // boom, draw.
            ::vkCmdDrawIndexed(
                command_buffer,
                static_cast<uint32_t>(index_buffer.count()),
                1u, 0u, 0u, 0u
            );

        ::vkCmdEndRenderPass(command_buffer);

        result = ::vkEndCommandBuffer(command_buffer);
        if(result != ::VK_SUCCESS) {
            CONSOLE_ERROR("Failed to record to command buffer.");
            return false;
        }

        // now wait again, but this time the signal and wait semephores are
        // reversed
        ::VkSemaphore wait_sems[] = {
            _image_available_sems[frame_index]
        };

        ::VkPipelineStageFlags wait_stage_masks[] {
            ::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        ::VkSemaphore signal_sems[] = {
            _draw_complete_sems[frame_index]
        };

        ::VkSubmitInfo submit_info { };
        submit_info.sType = ::VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount =
            static_cast<uint32_t>(std::size(wait_sems));
        submit_info.pWaitSemaphores = wait_sems;
        submit_info.pWaitDstStageMask = wait_stage_masks;
        submit_info.commandBufferCount = 1u;
        submit_info.pCommandBuffers = &_queues.command_buffer(image_index);
        submit_info.signalSemaphoreCount =
            static_cast<uint32_t>(std::size(signal_sems));
        submit_info.pSignalSemaphores = signal_sems;

        // submit the graphics command buffer
        result = ::vkQueueSubmit(
            _queues.graphics_queue(),
            1u,
            &submit_info,
            _display_fences[frame_index]
        );

        if(result != ::VK_SUCCESS) {
            CONSOLE_ERROR("Could not submit command queue.");
            return false;
        }

        ::VkSwapchainKHR swapchains[] = {
            swapchain.swapchain()
        };

        // notify the present buffer that we're going to wait for the current
        // frame to finsh/for the next vertical refresh
        ::VkPresentInfoKHR present_info { };
        present_info.sType = ::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 
            static_cast<uint32_t>(std::size(signal_sems));
        present_info.pWaitSemaphores = signal_sems;
        present_info.swapchainCount =
            static_cast<uint32_t>(std::size(swapchains));
        present_info.pSwapchains = swapchains;
        present_info.pImageIndices = &image_index;
        present_info.pResults = nullptr;

        // once more, do the thing and check to see if anything funky happened
        // along the way
        result = ::vkQueuePresentKHR(_queues.present_queue(), &present_info);
        if(result == ::VK_ERROR_OUT_OF_DATE_KHR ||
           result == ::VK_SUBOPTIMAL_KHR)
        {
            _image_resized(instance, swapchain, pipeline, framebuffers);
        }
    }

    ::vkDeviceWaitIdle(_device);

    return false;
}

// =============================================================================
void RenderLoop::init_synchronization() {
    CONSOLE_INFO("");

    ::VkSemaphoreCreateInfo sem_info { };
    sem_info.sType = ::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    ::VkResult result = ::VK_RESULT_MAX_ENUM;

    // the semephores which will let us know when the swapchain has finished
    // whatever it was doing with one of the images
    for(auto &sem : _image_available_sems) {
        result = ::vkCreateSemaphore(_device, &sem_info, nullptr, &sem);
        if(result != ::VK_SUCCESS) {
            CONSOLE_ERROR("Could not create image available semaphore");
        }
    }

    // the semephores letting us know when a draw has completed to the back
    // buffer/image
    for(auto &sem : _draw_complete_sems) {
        result = ::vkCreateSemaphore(_device, &sem_info, nullptr, &sem);
        if(result != ::VK_SUCCESS) {
            CONSOLE_ERROR("Could not create draw complete semaphore");
        }
    }

    // the crudest of the three - the vertical refresh fences; once there's a
    // frame being written to the monitor and a frame on the back buffer, just
    // hold your horses
    ::VkFenceCreateInfo fence_info { };
    fence_info.sType = ::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = ::VK_FENCE_CREATE_SIGNALED_BIT;

    for(auto &fence : _display_fences) {
        result = ::vkCreateFence(_device, &fence_info, nullptr, &fence);
        if(result != ::VK_SUCCESS) {
            CONSOLE_ERROR("Could not create display fence");
        }
    }

    CONSOLE_TRACE("Created synchronization primitives");
}

// =============================================================================
void RenderLoop::_image_resized(const Instance &instance, Swapchain &swapchain,
                                Pipeline &pipeline, Framebuffers &framebuffers)
{
    CONSOLE_WARN("Image requires updating");

    // wait for current commands to run their course
    ::vkDeviceWaitIdle(instance.logical_device());

    // demolish it all, then recreate it per the dependencies established
    // initially
    framebuffers.destroy();
    swapchain.destroy();
    _window.init_surface();
    swapchain.create({ _window.width(), _window.height() }, _queues,
                     _window.surface());
    framebuffers.create(swapchain, pipeline);
    pipeline.update_dimensions(swapchain);
}

// =============================================================================
void RenderLoop::_update_ubo(UniformBufferObject &ubo,
                             const Swapchain &swapchain,
                             const uint32_t image_index)
{
    using HRC = std::chrono::high_resolution_clock;
    using second_period = std::chrono::seconds::period;
    using duration_seconds = std::chrono::duration<float, second_period>;

    static auto start = HRC::now();
    auto now = HRC::now();
    auto runtime = duration_seconds(now - start).count();

    MVPMatrices matrices { };

    matrices.model = glm::rotate(
        glm::mat4(1.0f),
        runtime * 0.7854f,
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    matrices.view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    matrices.proj = glm::perspective(
        0.7854f,
        swapchain.aspect_ratio(),
        0.1f,
        10.0f
    );

    ubo.update(&matrices, image_index);
}

// =============================================================================
RenderLoop::RenderLoop(const ::VkDevice &device, Window &window,
                       CommandQueues &queues) :
    _device  { device },
    _window  { window },
    _queues  { queues }
{
    CONSOLE_INFO("");
}

RenderLoop::~RenderLoop() {
    CONSOLE_INFO("");

    for(auto &sem : _image_available_sems) {
        ::vkDestroySemaphore(_device, sem, nullptr);
    }
    
    for(auto &sem : _draw_complete_sems) {
        ::vkDestroySemaphore(_device, sem, nullptr);
    }
    
    for(auto &fence : _display_fences) {
        ::vkDestroyFence(_device, fence, nullptr);
    }
}