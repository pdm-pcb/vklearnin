#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/devices/CmdQueue.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void CmdQueue::fill_create_info(const uint32_t index, const float priority) {
    _index    = index;
    _priority = priority;

    _create_info = vk::DeviceQueueCreateInfo {
        .queueFamilyIndex = _index,
        .queueCount       = 1u,
        .pQueuePriorities = &_priority,
    };
}

// =============================================================================
void CmdQueue::request_queue() {
    _queue = LogicalDevice::native().getQueue(_index, 0u);
    if(!_queue) {
        CONSOLE_CRITICAL("Could not get device queue");
    }
}

CmdQueue::CmdQueue() :
    _index    { std::numeric_limits<uint32_t>::max() },
    _priority { std::numeric_limits<float>::max() },
    _queue    { }
{ }

} // namespace vkl