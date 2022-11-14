#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/Renderer.hpp"

#include "vklearnin/rendering/GraphicsInstance.hpp"
#include "vklearnin/rendering/LogicalDevice.hpp"
#include "vklearnin/rendering/Framebuffer.hpp"
#include "vklearnin/rendering/DeviceQueue.hpp"
#include "vklearnin/rendering/Swapchain.hpp"
#include "vklearnin/rendering/Pipeline.hpp"

#include "vklearnin/system/Win32TargetWindow.hpp"

#include "vklearnin/buffers/BufferObject.hpp"

namespace vkl {

#if defined(__linux__)
    using TargetWindow = XCBTargetWindow;
#elif defined(_WIN32)
    using TargetWindow = Win32TargetWindow;
#endif

// =============================================================================
void Renderer::draw(const BufferObject &vertex_buffer,
                    const uint32_t vertex_count)
{
    _next_swapchain_image_index();

    if(_target_image_index == std::numeric_limits<uint32_t>::max()) {
        _image_resized();
        return;
    }

    // clear out what needs clearing
    auto result = _logical_device.native().resetFences(
        1u,
        &_present_fences[_current_framebuffer]
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Could not reset device fences");
    }

    auto &graphics_queue = _logical_device.graphics_queue();
    graphics_queue.reset_cmd_pool(_target_image_index);

    vk::CommandBufferBeginInfo begin_info { };

    auto command_buffer = graphics_queue.cmd_buffer(_target_image_index);
    result = command_buffer.begin(begin_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR("Failed to begin command buffer recording.");
    }

    // bind the pipeline so everything's current
    command_buffer.bindPipeline(
        vk::PipelineBindPoint::eGraphics,
        _pipeline.native()
    );

    // initial setup for the pass
    vk::ClearValue clear_values[] = {
        { .color { std::array<float, 4> { 0.01f, 0.01f, 0.02f, 1.0f }}}
    };

    vk::RenderPassBeginInfo pass_info {
        .renderPass      = _pipeline.renderpass(),
        .framebuffer     = _framebuffers[_target_image_index].native(),
        .renderArea      = _swapchain.render_area(),
        .clearValueCount = static_cast<uint32_t>(std::size(clear_values)),
        .pClearValues    = clear_values,
    };

    // go time!
    command_buffer.beginRenderPass(pass_info, vk::SubpassContents::eInline);

        // set the dynamic traits of the pipeline
        command_buffer.setViewport(0u, 1u, &_pipeline.viewport());
        command_buffer.setScissor(0u, 1u, &_pipeline.scissor());

        // bind the VBO/IBO
        command_buffer.bindVertexBuffers(
            0u,
            { vertex_buffer.buffer },
            { 0u }
        );

        // draw!
        command_buffer.draw(
            vertex_count,
            1u, // instance count
            0u, // first vertex
            0u // first instance
        );

    command_buffer.endRenderPass();
    result = command_buffer.end();
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR("Failed to end command buffer recording.");
    }

    vk::Semaphore wait_sems[] = {
        _image_available_sems[_current_framebuffer]
    };

    vk::PipelineStageFlags wait_stage_masks[] {
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    };

    vk::Semaphore signal_sems[] = {
        _draw_complete_sems[_current_framebuffer]
    };

    vk::SubmitInfo submit_info {
        .waitSemaphoreCount = static_cast<uint32_t>(std::size(wait_sems)),
        .pWaitSemaphores = wait_sems,
        .pWaitDstStageMask = wait_stage_masks,
        .commandBufferCount = 1u,
        .pCommandBuffers = &command_buffer,
        .signalSemaphoreCount =
            static_cast<uint32_t>(std::size(signal_sems)),
        .pSignalSemaphores = signal_sems,
    };

    // submit the graphics command buffer
    result = graphics_queue.native(_target_image_index).submit(
        submit_info,
        _present_fences[_current_framebuffer]
    );
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR("Could not submit command buffer to graphics queue.");
    }
}

// =============================================================================
void Renderer::present() {
    auto &present_queue =
        _logical_device.present_queue().native(_target_image_index);

    vk::Semaphore signal_sems[] = {
        _draw_complete_sems[_current_framebuffer]
    };

    // notify the present buffer that we're going to wait for the current
    // frame to finsh/for the next vertical refresh
    std::vector<vk::SwapchainKHR> swapchains { _swapchain.native() };
    std::vector<vk::Result> present_results;
    vk::PresentInfoKHR present_info {
        .waitSemaphoreCount = static_cast<uint32_t>(std::size(signal_sems)),
        .pWaitSemaphores = signal_sems,
        .swapchainCount = static_cast<uint32_t>(swapchains.size()),
        .pSwapchains = swapchains.data(),
        .pImageIndices = &_target_image_index,
        .pResults = present_results.data()
    };

    // once more, do the thing and check to see if anything funky happened
    // along the way
    auto result = present_queue.presentKHR(present_info);
    for(const auto &present_result : present_results) {
        if(present_result == vk::Result::eErrorOutOfDateKHR ||
            present_result == vk::Result::eSuboptimalKHR)
        {
            CONSOLE_WARN("presentKHR() returned '{}'", to_string(result));
            _image_resized();
            return;
        }
    }
}

// =============================================================================
void Renderer::init() {
    // Set aside the room for image-count-number of synchronization primitives
    _image_available_sems.resize(RenderConfig::swapchain_image_count);
    _draw_complete_sems.resize(RenderConfig::swapchain_image_count);
    _present_fences.resize(RenderConfig::swapchain_image_count);

    _current_framebuffer = 0u;
    _target_image_index  = 0u;

    // Tie the framebuffers to the swapchain images
    for(uint32_t image_idx = 0;
        image_idx < RenderConfig::swapchain_image_count;
        ++image_idx)
    {
        _framebuffers[image_idx].create(image_idx);
    }

    vk::SemaphoreCreateInfo sem_info { };
    vk::Result result;

    // the semephores which will let us know when the swapchain has finished
    // whatever it was doing with one of the images
    for(auto &sem : _image_available_sems) {
        std::tie(result, sem) =
            _logical_device.native().createSemaphore(sem_info);
        if(result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL("Unable to create image available semaphore");
        }
    }

    // the semephores letting us know when a draw has completed to the back
    // buffer/image
    for(auto &sem : _draw_complete_sems) {
        std::tie(result, sem) =
            _logical_device.native().createSemaphore(sem_info);
        if(result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL("Unable to create draw complete semaphore");
        }
    }

    // Once there's a frame being written to the monitor and a frame on the
    // back buffer, the CPU needs to wait on the GPU before more frames can be
    // submitted.
    vk::FenceCreateInfo fence_info {
        .flags = vk::FenceCreateFlagBits::eSignaled
    };

    for(auto &fence : _present_fences) {
        std::tie(result, fence) =
            _logical_device.native().createFence(fence_info);
        if(result != vk::Result::eSuccess) {
            CONSOLE_CRITICAL("Unable to create display fence");
        }
    }

    CONSOLE_TRACE("Created synchronization primitives");
}

// =============================================================================
void Renderer::shutdown() {
    auto result = _logical_device.native().waitIdle();
    if(result != vk::Result::eSuccess) {
        CONSOLE_ERROR("Unable to wait for device idle on engine shutdown.");
    }
    
    for(auto &sem : _image_available_sems) {
        _logical_device.native().destroy(sem);
    }
    
    for(auto &sem : _draw_complete_sems) {
        _logical_device.native().destroy(sem);
    }
    
    for(auto &fence : _present_fences) {
        _logical_device.native().destroy(fence);
    }

    for(uint32_t image_idx = 0;
        image_idx < RenderConfig::swapchain_image_count;
        ++image_idx)
    {
        _framebuffers[image_idx].destroy();
    }
}

// =============================================================================
void Renderer::_next_swapchain_image_index() {
    // flip between zero and one, without a mod operation
    // courtesy paxdiablo: https://stackoverflow.com/a/4084058/1464937
    _current_framebuffer = 1 - _current_framebuffer;

    // wait for an image to become available to write to
    auto result = _logical_device.native().waitForFences(
        1u, // fence count
        &_present_fences[_current_framebuffer], // which fences to wait on
        VK_TRUE, // signal on all or any of the fences?
        std::numeric_limits<int64_t>::max() // block effectively forever
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("vk::Device::waitForFences() failed");
    }

    // since the fence signaled, we can now ask the swapchain which image it'd
    // like us to write to
    _target_image_index = _swapchain.next_image_index(
        _image_available_sems[_current_framebuffer]
    );
}

// =============================================================================
void Renderer::_image_resized() {
    // wait for current commands to run their course
    auto result = _logical_device.native().waitIdle();
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to wait for device idle on image resize.");
    }

    CONSOLE_WARN("Destroy framebuffers");
    for(uint32_t image_idx = 0;
        image_idx < RenderConfig::swapchain_image_count;
        ++image_idx)
    {
        _framebuffers[image_idx].destroy();
    }

    CONSOLE_WARN("Destroy swapchain");
    _swapchain.destroy();
    
    CONSOLE_WARN("Reinitialize window surface");
    TargetWindow::init_surface(_graphics_instance.native());

    CONSOLE_WARN("Recreate swapchain");
    _swapchain.create();

    CONSOLE_WARN("Update pipeline dimensions");
    _pipeline.update_dimensions();

    CONSOLE_WARN("Recreate framebuffers");
    for(uint32_t image_idx = 0;
        image_idx < RenderConfig::swapchain_image_count;
        ++image_idx)
    {
        _framebuffers[image_idx].create(image_idx);
    }
}

// =============================================================================
Renderer::Renderer(GraphicsInstance &graphics_instance,
                   LogicalDevice &logical_device,
                   Swapchain &swapchain, Pipeline &pipeline) :
    _current_framebuffer { std::numeric_limits<uint32_t>::max() },
    _target_image_index  { std::numeric_limits<uint32_t>::max() },
    _graphics_instance   { graphics_instance },
    _logical_device      { logical_device },
    _swapchain           { swapchain },
    _pipeline            { pipeline }
{
    _framebuffers.reserve(RenderConfig::swapchain_image_count);
    for(uint32_t image_idx = 0;
        image_idx < RenderConfig::swapchain_image_count;
        ++image_idx)
    {
        _framebuffers.emplace_back(
            _logical_device.native(),
            _swapchain,
            _pipeline
        );
    }
}

} // namespace vkl