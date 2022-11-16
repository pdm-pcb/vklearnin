#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/devices/CmdQueue.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"

namespace vkl {

void CmdQueue::request_queue() {
    _create_info = vk::DeviceQueueCreateInfo {
        .queueFamilyIndex = _family_index,
        .queueCount = 1u,
        .pQueuePriorities = &_priority,
    };

    _queue = LogicalDevice::native().getQueue(_family_index, 0u);
    if(!_queue) {
        CONSOLE_CRITICAL("Could not get device queue");
    }
}

CmdQueue::CmdQueue(const uint32_t family_index, const float priority) :
    _family_index { family_index },
    _priority     { priority },
    _create_info  { },
    _queue        { nullptr }
{ }

} // namespace vkl