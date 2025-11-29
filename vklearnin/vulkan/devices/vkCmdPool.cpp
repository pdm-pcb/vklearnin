#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/devices/vkCmdPool.hpp"

#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/devices/vkQueue.hpp"

namespace vkl {

// =============================================================================
vkCmdPool::vkCmdPool(vkCmdPool &&other) :
    _handle { other._handle },
    _device { other._device }
{
    other._handle = nullptr;
    other._device = nullptr;
}

// =============================================================================
bool vkCmdPool::create(vkDevice const &device,
                       vk::CommandPoolCreateInfo const &create_info)
{
    if(_handle) {
        Log::error("Command pool {} already exists", _handle);
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create command pool with invalid device.");
        return false;
    }

    _device = device.native();

    // vk::CommandPoolCreateInfo const pool_info {
    //     .pNext = nullptr,
    //     .flags = flags,
    //     .queueFamilyIndex = queue_index,
    // };

    _handle = _device.createCommandPool(create_info);
    Log::trace("Created command pool {}", _handle);

    return true;
}

// =============================================================================
bool vkCmdPool::destroy() {
    if(!_handle) {
        Log::error("Must create command pool before calling destroy.");
        return false;
    }

    Log::trace("Destroying command pool {}", _handle);
    _device.destroyCommandPool(_handle);
    _handle = nullptr;

    return true;
}

// =============================================================================
void vkCmdPool::reset(vk::CommandPoolResetFlags const flags) const {
    _device.resetCommandPool(_handle, flags);
}

} // namespace vkl