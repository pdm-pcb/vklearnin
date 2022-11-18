#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/devices/CmdQueue.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

void CmdQueue::populate_info(const uint32_t index, const float priority) {
    _index = index;
    _priority = priority;

    _create_info = vk::DeviceQueueCreateInfo {
        .queueFamilyIndex = _index,
        .queueCount = 1u,
        .pQueuePriorities = &_priority,
    };
}

void CmdQueue::request_queue() {
    _queue = LogicalDevice::native().getQueue(_index, 0u);
    if(!_queue) {
        CONSOLE_CRITICAL("Could not get device queue");
    }
}

CmdQueue::CmdQueue() :
    _queue { nullptr }
{ }

} // namespace vkl