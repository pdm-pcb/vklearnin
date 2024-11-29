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
bool vkQueue::submit(CmdBuffers const cmd_buffers,
                     WaitSemaphores const wait_sems,
                     WaitStageFlags const wait_stage_flags,
                     SignalSemaphores const signal_sems,
                     vk::Fence const &in_flight_fence) const
{
    vk::SubmitInfo const submit_info {
        .pNext                = nullptr,
        .waitSemaphoreCount   = static_cast<uint32_t>(wait_sems.size()),
        .pWaitSemaphores      = wait_sems.data(),
        .pWaitDstStageMask    = wait_stage_flags.data(),
        .commandBufferCount   = static_cast<uint32_t>(cmd_buffers.size()),
        .pCommandBuffers      = cmd_buffers.data(),
        .signalSemaphoreCount = static_cast<uint32_t>(signal_sems.size()),
        .pSignalSemaphores    = signal_sems.data(),
    };

    auto const result = _handle.submit(submit_info, in_flight_fence);
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
bool vkQueue::submit(CmdBuffers const cmd_buffers) const {
    vk::SubmitInfo const submit_info {
        .pNext                = nullptr,
        .waitSemaphoreCount   = 0u,
        .pWaitSemaphores      = nullptr,
        .pWaitDstStageMask    = nullptr,
        .commandBufferCount   = static_cast<uint32_t>(cmd_buffers.size()),
        .pCommandBuffers      = cmd_buffers.data(),
        .signalSemaphoreCount = 0u,
        .pSignalSemaphores    = nullptr,
    };

    auto const result = _handle.submit(submit_info);
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