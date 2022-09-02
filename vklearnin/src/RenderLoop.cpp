#include "vklearnin/common.hpp"
#include "vklearnin/RenderLoop.hpp"

#include "vklearnin/Instance.hpp"
#include "vklearnin/CommandStructures/CommandQueues.hpp"
#include "vklearnin/Swapchain.hpp"
#include "vklearnin/Pipeline.hpp"
#include "vklearnin/Buffers/Framebuffers.hpp"
#include "vklearnin/Shaders/Buffers/BufferObject.hpp"
#include "vklearnin/Shaders/Buffers/UniformBufferObject.hpp"
#include "vklearnin/DescriptorSets/PerFrameDescriptors.hpp"
#include "vklearnin/DescriptorSets/PerMaterialDescriptors.hpp"
#include "vklearnin/Models/Model.hpp"

#if defined(__linux__)
    #include "vklearnin/Platform/X11/X11Window.hpp"
#elif defined(_WIN32)
    #include "vklearnin/Platform/Win32/Win32Window.hpp"
#endif

// =============================================================================
bool RenderLoop::run(const Instance &instance, Swapchain &swapchain,
                     UniformBufferObject &ubo, Pipeline &pipeline,
                     Framebuffers &framebuffers,
                     PerFrameDescriptors &per_frame_descriptors,
                     PerMaterialDescriptors &per_material_descriptors,
                     const std::vector<Model *> &models)
{
    CONSOLE_INFO("");

    uint32_t frame_index = 0u;
    uint32_t image_index = 0u;

    while(_window.message_loop() == true) {
        using HRC = std::chrono::high_resolution_clock;
        using second_period = std::chrono::seconds::period;
        using duration_seconds = std::chrono::duration<float, second_period>;

        static auto start = HRC::now();
        auto now = HRC::now();
        auto runtime = duration_seconds(now - start).count();

        // flip between zero and one, without a mod operation
        // courtesy paxdiablo: https://stackoverflow.com/a/4084058/1464937
        frame_index = 1 - frame_index;

        // wait your turn
        auto result = _device.waitForFences(1u, &_display_fences[frame_index],
                                            VK_TRUE, UI64MAX);

        if(result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL("Device::waitForFences() failed");
        }

        // grab the next swapchain image and check it...
        result = _device.acquireNextImageKHR(
            swapchain.swapchain(),
            UI64MAX,
            _image_available_sems[frame_index],
            nullptr,
            &image_index
        );

        // if we need to resize everything, let's do it
        if(result == vk::Result::eErrorOutOfDateKHR) {
            _image_resized(instance, swapchain, pipeline, framebuffers);
            continue;   // be sure to continue so eveything updates
        }

        // now send the fresh data to the UBOs. Im curious: how much does order
        // matter here? Should this be done after the fences are reset? Is
        // that even relevant? Guess I'll have to read the spec. =)
        _update_ubo(ubo, swapchain, frame_index);

        // clear out what needs clearing
        result = _device.resetFences(1u, &_display_fences[frame_index]);
        if(result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL("Could not reset device fences");
        }

        _queues.reset_command_buffer(
            image_index,
            static_cast<vk::CommandBufferResetFlagBits>(0u)
        );

        vk::CommandBufferBeginInfo begin_info { };

        auto command_buffer = _queues.command_buffer(image_index);
        command_buffer.begin(begin_info);

        // bind the pipeline so everything's current
        command_buffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics,
            pipeline.pipeline()
        );

        command_buffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            pipeline.layout(),
            0u, 1u,
            &per_frame_descriptors.sets()[frame_index],
            0u,
            nullptr
        );

        // initial setup for the pass
        vk::ClearValue clear_values[] = {
            { .color { std::array<float, 4> { 0.01f, 0.01f, 0.01f, 1.0f }}},
            { .depthStencil  { 1.0f, 0u }}
        };

        vk::RenderPassBeginInfo pass_info {
            .renderPass = pipeline.renderpass(),
            .framebuffer = framebuffers.buffer(image_index),
            .renderArea = swapchain.render_area(),
            .clearValueCount = static_cast<uint32_t>(std::size(clear_values)),
            .pClearValues = clear_values,
        };

        // go time!
        command_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);

            // update the dynamic traits of the pipeline
            command_buffer.setViewport(0u, 1u, &pipeline.viewport());
            command_buffer.setScissor(0u, 1u, &pipeline.scissor());

            for(size_t model_idx = 0; model_idx < models.size(); ++model_idx) {
                // time for some host-side vertex data!
                command_buffer.bindVertexBuffers(
                    0u,
                    models[model_idx]->vertex_buffers(),
                    models[model_idx]->vertex_buffer_offsets()
                );
                
                // and indices while we're at it
                command_buffer.bindIndexBuffer(
                    models[model_idx]->index_buffer(),
                    0u,
                    IndexType
                );

                command_buffer.bindDescriptorSets(
                    vk::PipelineBindPoint::eGraphics,
                    pipeline.layout(),
                    1u, 1u,
                    &per_material_descriptors.sets(model_idx)[frame_index],
                    0u,
                    nullptr
                );

                command_buffer.pushConstants(
                    pipeline.layout(),
                    vk::ShaderStageFlagBits::eVertex,
                    0u,
                    sizeof(glm::mat4),
                    &models[model_idx]->update_model_matrix(runtime)
                );

                // boom, draw.
                command_buffer.drawIndexed(
                    static_cast<uint32_t>(models[model_idx]->index_count()),
                    1u, 0u, 0u, 0u
                );
            }

        command_buffer.endRenderPass();
        command_buffer.end();

        // now wait again, but this time the signal and wait semephores are
        // reversed
        vk::Semaphore wait_sems[] = {
            _image_available_sems[frame_index]
        };

        vk::PipelineStageFlags wait_stage_masks[] {
            vk::PipelineStageFlagBits::eColorAttachmentOutput
        };

        vk::Semaphore signal_sems[] = {
            _draw_complete_sems[frame_index]
        };

        vk::SubmitInfo submit_info {
            .waitSemaphoreCount = static_cast<uint32_t>(std::size(wait_sems)),
            .pWaitSemaphores = wait_sems,
            .pWaitDstStageMask = wait_stage_masks,
            .commandBufferCount = 1u,
            .pCommandBuffers = &_queues.command_buffer(image_index),
            .signalSemaphoreCount =
                static_cast<uint32_t>(std::size(signal_sems)),
            .pSignalSemaphores = signal_sems,
        };

        // submit the graphics command buffer
        _queues.graphics_queue().submit(
            submit_info,
            _display_fences[frame_index]
        );

        vk::SwapchainKHR swapchains[] = {
            swapchain.swapchain()
        };

        // notify the present buffer that we're going to wait for the current
        // frame to finsh/for the next vertical refresh
        vk::PresentInfoKHR present_info {
            .waitSemaphoreCount = static_cast<uint32_t>(std::size(signal_sems)),
            .pWaitSemaphores = signal_sems,
            .swapchainCount = static_cast<uint32_t>(std::size(swapchains)),
            .pSwapchains = swapchains,
            .pImageIndices = &image_index,
        };

        // once more, do the thing and check to see if anything funky happened
        // along the way
        result = _queues.present_queue().presentKHR(present_info);
        if(result == vk::Result::eErrorOutOfDateKHR ||
           result == vk::Result::eSuboptimalKHR)
        {
            _image_resized(instance, swapchain, pipeline, framebuffers);
        }
    }

    _device.waitIdle();

    return false;
}

// =============================================================================
void RenderLoop::init_synchronization() {
    CONSOLE_INFO("");

    vk::SemaphoreCreateInfo sem_info { };

    // the semephores which will let us know when the swapchain has finished
    // whatever it was doing with one of the images
    for(auto &sem : _image_available_sems) {
        sem = _device.createSemaphore(sem_info);
    }

    // the semephores letting us know when a draw has completed to the back
    // buffer/image
    for(auto &sem : _draw_complete_sems) {
        sem = _device.createSemaphore(sem_info);
    }

    // the crudest of the three - the vertical refresh fences; once there's a
    // frame being written to the monitor and a frame on the back buffer, just
    // hold your horses
    vk::FenceCreateInfo fence_info {
        .flags = vk::FenceCreateFlagBits::eSignaled
    };

    for(auto &fence : _display_fences) {
        fence = _device.createFence(fence_info);
    }

    CONSOLE_TRACE("Created synchronization primitives");
}

// =============================================================================
void RenderLoop::_image_resized(const Instance &instance, Swapchain &swapchain,
                                Pipeline &pipeline, Framebuffers &framebuffers)
{
    CONSOLE_WARN("Image requires updating");

    // wait for current commands to run their course
    instance.logical_device().waitIdle();

    CONSOLE_TRACE("Destroy framebuffers and swapchain");
    framebuffers.destroy();
    swapchain.destroy();
    
    CONSOLE_TRACE("Reinitialize window surface");
    _window.init_surface();

    CONSOLE_TRACE("Recreate swapchain");
    swapchain.create(
        { _window.width(), _window.height() },
        _queues,
        _window.surface()
    );

    CONSOLE_TRACE("Update pipeline dimensions");
    pipeline.update_dimensions(swapchain);

    CONSOLE_TRACE("Recreate framebuffers");
    framebuffers.create(swapchain, pipeline);
}

// =============================================================================
void RenderLoop::_update_ubo(UniformBufferObject &ubo,
                             const Swapchain &swapchain,
                             const uint32_t image_index)
{
    VPMatrices matrices { };

    matrices.view = glm::lookAt(
        glm::vec3(0.0f, 2.0f, 20.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    matrices.proj = glm::perspective(
        0.7854f,
        swapchain.aspect_ratio(),
        0.1f,
        1000.0f
    );

    ubo.update(&matrices, image_index);
}

// =============================================================================
RenderLoop::RenderLoop(const vk::Device &device, Window &window,
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
        _device.destroy(sem);
    }
    
    for(auto &sem : _draw_complete_sems) {
        _device.destroy(sem);
    }
    
    for(auto &fence : _display_fences) {
        _device.destroy(fence);
    }
}