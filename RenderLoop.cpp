#include "common.hpp"
#include "RenderLoop.hpp"

#include "Instance.hpp"
#include "CommandQueues.hpp"
#include "Swapchain.hpp"
#include "Pipeline.hpp"
#include "Framebuffers.hpp"

#if defined(__linux__)
    #include "X11Window.hpp"
#elif defined(_WIN32)
    #include "Win32Window.hpp"
#endif

// =============================================================================
bool RenderLoop::run(const Instance &instance, Swapchain &swapchain,
                     Pipeline &pipeline, Framebuffers &framebuffers)
{
    ::VkResult result    = ::VK_RESULT_MAX_ENUM;
    uint32_t frame       = 0u;
    uint32_t image_index = 0u;

    while(_window.message_loop(*this) == true) {
        // flip between zero and one, without a mod operation
        // courtesy paxdiablo: https://stackoverflow.com/a/4084058/1464937
        frame = 1 - frame;

        ::vkWaitForFences(_device, 1u, &_display_fences[frame], VK_TRUE,
                          UI64MAX);

        result = instance._AcquireNextImageKHR(
            _device,
            swapchain.swapchain(),
            UI64MAX,
            _image_available_sems[frame],
            nullptr,
            &image_index
        );

        if(result == ::VK_ERROR_OUT_OF_DATE_KHR || _resized) {
            _image_resized(instance, swapchain, pipeline, framebuffers);
            continue;
        }

        ::vkResetFences(_device, 1u, &_display_fences[frame]);

        _queues.reset_command_buffer(
            image_index,
            static_cast<::VkCommandBufferResetFlagBits>(0u)
        );

        ::VkCommandBufferBeginInfo buffer_info { };
        buffer_info.sType = ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        buffer_info.flags = 0u;
        buffer_info.pInheritanceInfo = nullptr;

        auto buffer = _queues.command_buffer(image_index);
        result = ::vkBeginCommandBuffer(buffer, &buffer_info);

        if(result != ::VK_SUCCESS) {
            CONSOLE_CRITICAL("Unable to begin command buffer recording.");
        }

        ::VkRenderPassBeginInfo pass_info { };
        pass_info.sType = ::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        pass_info.renderPass = pipeline.renderpass();
        pass_info.framebuffer = framebuffers.buffer(image_index);

        auto [width, height] = swapchain.extent();
        auto [x, y]          = swapchain.offset();
        pass_info.renderArea.extent = { width, height };
        pass_info.renderArea.offset = { x, y };

        ::VkClearValue clear_values[] = {{
            .color = { 0.1f, 0.1f, 0.1f, 1.0f }
        }};

        pass_info.clearValueCount =
            static_cast<uint32_t>(std::size(clear_values));
        pass_info.pClearValues = clear_values;

        ::vkCmdBeginRenderPass(
            buffer,
            &pass_info,
            ::VK_SUBPASS_CONTENTS_INLINE
        );

                ::vkCmdBindPipeline(
                    buffer,
                    ::VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipeline.pipeline()
                );
                ::vkCmdSetViewport(buffer, 0u, 1u, pipeline.viewports());
                ::vkCmdSetScissor(buffer,0u, 1u, pipeline.scissors());
                ::vkCmdDraw(
                    buffer,
                    3u, 1u,
                    0u, 0u
                );

        ::vkCmdEndRenderPass(buffer);

        result = ::vkEndCommandBuffer(buffer);
        if(result != ::VK_SUCCESS) {
            CONSOLE_CRITICAL("Failed to record to command buffer.");
        }

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

        result = ::vkQueuePresentKHR(_queues.present_queue(), &present_info);
        if(result == ::VK_ERROR_OUT_OF_DATE_KHR ||
           result == ::VK_SUBOPTIMAL_KHR || _resized)
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
    ::VkSemaphoreCreateInfo sem_info { };
    sem_info.sType = ::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    ::VkResult result = ::VK_RESULT_MAX_ENUM;

    for(auto &sem : _image_available_sems) {
        result = ::vkCreateSemaphore(_device, &sem_info, nullptr, &sem);
        if(result != ::VK_SUCCESS) {
            CONSOLE_CRITICAL("Could not create image available semaphore");
        }
    }

    for(auto &sem : _draw_complete_sems) {
        result = ::vkCreateSemaphore(_device, &sem_info, nullptr, &sem);
        if(result != ::VK_SUCCESS) {
            CONSOLE_CRITICAL("Could not create draw complete semaphore");
        }
    }

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
    ::vkDeviceWaitIdle(instance.logical_device());
    _resized = false;

    framebuffers.destroy();
    swapchain.destroy();
    swapchain.create(_window.extent(), _queues);
    framebuffers.create(swapchain, pipeline);
    pipeline.update_dimensions(swapchain);
}

// =============================================================================
RenderLoop::RenderLoop(const ::VkDevice &device, Window &window,
                       CommandQueues &queues) :
    _resized { false  },
    _device  { device },
    _window  { window },
    _queues  { queues }
{
    CONSOLE_INFO("");
}

RenderLoop::~RenderLoop() {
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