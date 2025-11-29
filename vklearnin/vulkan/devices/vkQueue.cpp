#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/devices/vkQueue.hpp"

#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"
#include "vklearnin/vulkan/swapchain/vkSwapchain.hpp"

namespace vkl {

// =============================================================================
bool vkQueue::set(vkDevice const &device, uint32_t const family_index) {
    if(_handle || _family_index != std::numeric_limits<uint32_t>::max()) {
        Log::error("Queue {}, index {} already exists", _handle, _family_index);
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot get queue from invalid device.");
        return false;
    }

    _family_index = family_index;
    _handle = device.native().getQueue(_family_index, 0u);

    if(!_handle) {
        Log::error("Failed to get device queue, index {}.", _family_index);
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

bool vkQueue::submit(vk::SubmitInfo const &submit_info,
                     vk::Fence const &fence) const
{
    _handle.submit(submit_info, fence);
    return true;
}

// =============================================================================
bool vkQueue::submit(vk::SubmitInfo const &submit_info) const {
    _handle.submit(submit_info);
    return true;
}

// =============================================================================
bool vkQueue::present(vk::PresentInfoKHR const &present_info) const {
    auto const result = _handle.presentKHR(present_info);
    if(result != vk::Result::eSuccess) {
        Log::warn("Queue {} failed to present swapchain image: '{}'",
                  _handle,
                  vk::to_string(result));
        return false;
    }

    return true;
}

} // namespace vkl