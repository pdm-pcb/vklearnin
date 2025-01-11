#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/devices/vkQueue.hpp"

#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"
#include "vklearnin/vulkan/swapchain/vkSwapchain.hpp"

namespace vkl {

// =============================================================================
bool vkQueue::set(vkDevice const &device, uint32_t const family_index) {
    if(_handle || _family_index != std::numeric_limits<uint32_t>::max()) {
        Log::error(
            "Queue {}, index {} already exists",
            _handle,
            _family_index
        );
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot get queue from invalid device.");
        return false;
    }

    _family_index = family_index;
    _handle = device.native().getQueue(_family_index, 0u);

    if(!_handle) {
        Log::error("Failed to get device queue, index {}.",
                          _family_index);
        return false;
    }

    Log::trace("Got queue {}, index {}.", _handle, _family_index);
    return true;
}

// =============================================================================
bool vkQueue::clear() {
    if(!_handle || _family_index == std::numeric_limits<uint32_t>::max()) {
        Log::error("Must set queue before calling clear.");
        return false;
    }

    Log::trace("Clearing queue {}, index {}", _handle, _family_index);
    _handle = nullptr;
    _family_index = std::numeric_limits<uint32_t>::max();

    return true;
}

// =============================================================================
bool vkQueue::submit(vk::CommandBuffer const &cmd_buffer,
                     vk::Semaphore const &wait_semaphore,
                     vk::PipelineStageFlagBits2KHR const wait_stage_flags,
                     vk::Semaphore const &signal_semaphore,
                     vk::PipelineStageFlagBits2KHR const signal_stage_flags,
                     vk::Fence const &in_flight_fence) const
{
    vk::CommandBufferSubmitInfo const cmd_submit_info {
        .pNext = nullptr,
        .commandBuffer = cmd_buffer,
        .deviceMask = 0u,
    };

    vk::SemaphoreSubmitInfoKHR const wait_info {
        .pNext = nullptr,
        .semaphore = wait_semaphore,
        .value = 0u,
        .stageMask = wait_stage_flags,
        .deviceIndex = 0u,
    };

    vk::SemaphoreSubmitInfoKHR const signal_info {
        .pNext = nullptr,
        .semaphore = signal_semaphore,
        .value = 0u,
        .stageMask = signal_stage_flags,
        .deviceIndex = 0u,
    };

    vk::SubmitInfo2KHR const queue_submit_info {
      .pNext = nullptr,
      .flags = { },
      .waitSemaphoreInfoCount = 1u,
      .pWaitSemaphoreInfos = &wait_info,
      .commandBufferInfoCount = 1u,
      .pCommandBufferInfos = &cmd_submit_info,
      .signalSemaphoreInfoCount = 1u,
      .pSignalSemaphoreInfos = &signal_info,
    };

    auto const result = _handle.submit2KHR(
        1u,
        &queue_submit_info,
        in_flight_fence
    );

    if(result != vk::Result::eSuccess) {
        Log::error(
            "Submission to queue {} failed: '{}'",
            _handle,
            vk::to_string(result)
        );
        return false;
    }

    return true;
}

// =============================================================================
bool vkQueue::submit(vk::CommandBuffer const &cmd_buffer) const {
    vk::CommandBufferSubmitInfo const cmd_submit_info {
        .pNext = nullptr,
        .commandBuffer = cmd_buffer,
        .deviceMask = { },
    };

    vk::SubmitInfo2KHR const queue_submit_info {
      .pNext = nullptr,
      .flags = { },
      .waitSemaphoreInfoCount = 0u,
      .pWaitSemaphoreInfos = nullptr,
      .commandBufferInfoCount = 0u,
      .pCommandBufferInfos = &cmd_submit_info,
      .signalSemaphoreInfoCount = 0u,
      .pSignalSemaphoreInfos = nullptr,
    };

    auto const result = _handle.submit2KHR(
        1u,
        &queue_submit_info,
        nullptr
    );

    if(result != vk::Result::eSuccess) {
        Log::error(
            "Submission to queue {} failed: '{}'",
            _handle,
            vk::to_string(result)
        );
        return false;
    }

    return true;
}

// =============================================================================
bool vkQueue::present(vkSwapchain const &swapchain,
                      vk::Semaphore const &wait_sem,
                      uint32_t const image_index) const
{
    // This present call will wait on the complete_sem to ensure the
    // submitted batch of commands has finished
    vk::PresentInfoKHR const present_info {
        .waitSemaphoreCount = 1u,
        .pWaitSemaphores    = &wait_sem,
        .swapchainCount     = 1u,
        .pSwapchains        = &swapchain.native(),
        .pImageIndices      = &image_index,
    };

    // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/vkQueuePresentKHR.html
    // Vulkan-HPP asserts when a function returns anything classified as an
    // error. And according to the docs, returning out-of-date means the
    // function failed. So to bypass as little of Vulkan-HPP as possible, I'm
    // createing a present info struct that'll satisfy the Vulkan C API for the
    // call to present.

    auto const present_info_c = VkPresentInfoKHR(present_info);

    auto const result =
        vk::Result(::vkQueuePresentKHR(_handle, &present_info_c));

    if(result != vk::Result::eSuccess) {
        Log::warn(
            "Queue {} failed to present swapchain image: '{}'",
            _handle,
            vk::to_string(result)
        );
        return false;
    }

    return true;
}

} // namespace vkl