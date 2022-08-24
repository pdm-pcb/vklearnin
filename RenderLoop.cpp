#include "common.hpp"
#include "RenderLoop.hpp"

#include "Instance.hpp"
#include "CommandQueues.hpp"
#include "Swapchain.hpp"
#include "Pipeline.hpp"
#include "Framebuffers.hpp"
#include "Vertex.hpp"
#include "StagedVertexBuffer.hpp"

#if defined(__linux__)
    #include "X11Window.hpp"
#elif defined(_WIN32)
    #include "Win32Window.hpp"
#endif

// =============================================================================
bool RenderLoop::run(const Instance &instance, const CommandQueues &queues,
                     Swapchain &swapchain, Pipeline &pipeline,
                     Framebuffers &framebuffers)
{
    CONSOLE_INFO("");

    ::VkResult result = ::VK_RESULT_MAX_ENUM;
    uint32_t frame       = 0u;
    uint32_t image_index = 0u;

    const std::vector<Vertex> vertices {
        {{ -0.5f, -0.5f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }},
        {{  0.5f, -0.5f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }},
        {{  0.5f,  0.5f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }},
        {{ -0.5f,  0.5f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }}
    };

    const std::vector<StagedVertexBuffer::Index> indices {
        0u, 1u, 2u,
        2u, 3u, 0u
    };
    
    StagedVertexBuffer vb(vertices, indices, instance);
    vb.populate_buffers(queues.command_pool(), queues.graphics_queue());

    ::VkBuffer vertex_buffers[] {
        { vb.vertex_handle() }
    };
    ::VkDeviceSize vertex_buffer_offsets[] {
        { 0u }
    };

    while(_window.message_loop() == true) {
        // flip between zero and one, without a mod operation
        // courtesy paxdiablo: https://stackoverflow.com/a/4084058/1464937
        frame = 1 - frame;

        // wait your turn
        ::vkWaitForFences(_device, 1u, &_display_fences[frame], VK_TRUE,
                          UI64MAX);

        // grab the next swapchain image and check it...
        result = instance._AcquireNextImageKHR(
            _device,
            swapchain.swapchain(),
            UI64MAX,
            _image_available_sems[frame],
            nullptr,
            &image_index
        );

        // if we need to resize everything, let's do it
        if(result == ::VK_ERROR_OUT_OF_DATE_KHR) {
            _image_resized(instance, swapchain, pipeline, framebuffers);
            continue;   // be sure to continue so eveything updates
        }

        // something tells me this could go outside the loop, as it never
        // changes, but it's fine for now
        ::VkCommandBufferBeginInfo buffer_info { };
        buffer_info.sType = ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // clear out what needs clearing
        ::vkResetFences(_device, 1u, &_display_fences[frame]);
        _queues.reset_command_buffer(
            image_index,
            static_cast<::VkCommandBufferResetFlagBits>(0u)
        );

        auto command_buffer = _queues.command_buffer(image_index);
        result = ::vkBeginCommandBuffer(command_buffer, &buffer_info);

        if(result != ::VK_SUCCESS) {
            CONSOLE_CRITICAL("Unable to begin command buffer recording.");
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
                std::size(vertex_buffers),
                vertex_buffers,
                vertex_buffer_offsets
            );
            // and indices while we're at it
            ::vkCmdBindIndexBuffer(
                command_buffer,
                vb.index_handle(),
                0u,
                vb.index_type()
            );

            // boom, draw.
            ::vkCmdDrawIndexed(
                command_buffer,
                vb.index_count(),
                1u, 0u, 0u, 0u
            );

        ::vkCmdEndRenderPass(command_buffer);

        result = ::vkEndCommandBuffer(command_buffer);
        if(result != ::VK_SUCCESS) {
            CONSOLE_CRITICAL("Failed to record to command buffer.");
        }

        // now wait again, but this time the signal and wait semephores are
        // reversed
        ::VkSemaphore wait_sems[] = {
            _image_available_sems[frame]
        };

        ::VkPipelineStageFlags wait_stage_masks[] {
            ::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        ::VkSemaphore signal_sems[] = {
            _draw_complete_sems[frame]
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
            _display_fences[frame]
        );

        if(result != ::VK_SUCCESS) {
            CONSOLE_CRITICAL("Could not submit command queue.");
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

        // static uint32_t frame_count = 0u;
        // CONSOLE_TRACE("Frame {}", ++frame_count);
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
            CONSOLE_CRITICAL("Could not create image available semaphore");
        }
    }

    // the semephores letting us know when a draw has completed to the back
    // buffer/image
    for(auto &sem : _draw_complete_sems) {
        result = ::vkCreateSemaphore(_device, &sem_info, nullptr, &sem);
        if(result != ::VK_SUCCESS) {
            CONSOLE_CRITICAL("Could not create draw complete semaphore");
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
            CONSOLE_CRITICAL("Could not create display fence");
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