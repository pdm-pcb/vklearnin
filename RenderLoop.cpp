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
bool RenderLoop::run(const Instance &instance, const Swapchain &swapchain,
                     const Pipeline &pipeline, const Framebuffers &framebuffers)
{
    ::VkResult result = ::VK_RESULT_MAX_ENUM;

    while(_window.message_loop() == true) {
        ::vkWaitForFences(_device, 1u, &_display_fence, VK_TRUE, UI64MAX);
        ::vkResetFences(_device, 1u, &_display_fence);

        uint32_t image_index = 0u;
        instance._AcquireNextImageKHR(
            _device,
            swapchain.swapchain(),
            UI64MAX,
            _image_available_sem,
            nullptr,
            &image_index
        );

        _queues.reset_command_buffer(
            static_cast<::VkCommandBufferResetFlagBits>(0u)
        );

        ::VkCommandBufferBeginInfo buffer_info { };
        buffer_info.sType = ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        buffer_info.flags = 0u;
        buffer_info.pInheritanceInfo = nullptr;

        auto buffer = _queues.command_buffer();
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
                ::vkCmdSetViewport(
                    buffer,
                    0u, 1u,
                    pipeline.viewports()
                );
                ::vkCmdSetScissor(
                    buffer,
                    0u, 1u,
                    pipeline.scissors()
                );
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
            _image_available_sem
        };

        ::VkPipelineStageFlags wait_stage_masks[] {
            ::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        ::VkSemaphore signal_sems[] = {
            _draw_complete_sem
        };

        ::VkSubmitInfo submit_info { };
        submit_info.sType = ::VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount =
            static_cast<uint32_t>(std::size(wait_sems));
        submit_info.pWaitSemaphores = wait_sems;
        submit_info.pWaitDstStageMask = wait_stage_masks;
        submit_info.commandBufferCount = 1u;
        submit_info.pCommandBuffers = &_queues.command_buffer();
        submit_info.signalSemaphoreCount =
            static_cast<uint32_t>(std::size(signal_sems));
        submit_info.pSignalSemaphores = signal_sems;

        result = ::vkQueueSubmit(
            _queues.graphics_queue(),
            1u,
            &submit_info,
            _display_fence
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

        ::vkQueuePresentKHR(_queues.present_queue(), &present_info);
    }

    ::vkWaitForFences(_device, 1u, &_display_fence, VK_TRUE, UI64MAX);

    return false;
}

// =============================================================================
void RenderLoop::init_synchronization() {
    ::VkSemaphoreCreateInfo sem_info { };
    sem_info.sType = ::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    ::VkResult result = ::vkCreateSemaphore(
        _device,
        &sem_info,
        nullptr,
        &_image_available_sem
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Could not create image available semaphore");
    }

    result = ::vkCreateSemaphore(
        _device,
        &sem_info,
        nullptr,
        &_draw_complete_sem
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Could not create draw complete semaphore");
    }

    ::VkFenceCreateInfo fence_info { };
    fence_info.sType = ::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = ::VK_FENCE_CREATE_SIGNALED_BIT;

    result = ::vkCreateFence(
        _device,
        &fence_info,
        nullptr,
        &_display_fence
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Could not create display fence");
    }

    CONSOLE_TRACE("Created synchronization primitives");
}

// =============================================================================
RenderLoop::RenderLoop(const ::VkDevice &device, Window &window,
                       CommandQueues &queues) :
    _image_available_sem { nullptr  },
    _draw_complete_sem   { nullptr  },
    _display_fence       { nullptr  },
    _device { device },
    _window { window },
    _queues { queues }
{
    CONSOLE_INFO("");
}

RenderLoop::~RenderLoop() {
    if(_image_available_sem != nullptr) {
        ::vkDestroySemaphore(
            _device,
            _image_available_sem,
            nullptr
        );
    }
    
    if(_draw_complete_sem != nullptr) {
        ::vkDestroySemaphore(
            _device,
            _draw_complete_sem,
            nullptr
        );
    }
    
    if(_display_fence != nullptr) {
        ::vkDestroyFence(
            _device,
            _display_fence,
            nullptr
        );
    }
}