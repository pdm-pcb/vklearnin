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
                       uint32_t const queue_index,
                       vk::CommandPoolCreateFlags const flags)
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

    vk::CommandPoolCreateInfo const pool_info {
        .flags = flags,
        .queueFamilyIndex = queue_index,
    };

    auto const result = _device.createCommandPool(
        &pool_info,
        nullptr,
        &_handle
    );

    if(result != vk::Result::eSuccess) {
        Log::error(
            "Failed to create command pool: '{}'",
            vk::to_string(result)
        );
        return false;
    }

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
bool vkCmdPool::reset(vk::CommandPoolResetFlags const flags) const {
    auto const result = _device.resetCommandPool(_handle, flags);

    if(result != vk::Result::eSuccess) {
        Log::error(
            "Failed to reset command buffer {}: '{}'",
             _handle,
             vk::to_string(result)
        );
        return false;
    }

    return true;
}

} // namespace vkl