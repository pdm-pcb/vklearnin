#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/devices/CmdPool.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/system/devices/CmdQueue.hpp"

namespace vkl {

// =============================================================================
void CmdPool::create(const vk::CommandPoolCreateFlags flags) {
    const vk::CommandPoolCreateInfo pool_info {
        .flags = flags,
        .queueFamilyIndex = LogicalDevice::cmd_queue().index()
    };

    auto const result = LogicalDevice::native().createCommandPool(
        &pool_info,
        nullptr,
        &_pool
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to create command pool: '{}'",
            to_string(result)
        );
    }
    else {
        CONSOLE_TRACE(
            "Created command pool {:#x}.",
            reinterpret_cast<uint64_t>(VkCommandPool(_pool))
        );
    }
}

// =============================================================================
void CmdPool::destroy() {
    CONSOLE_TRACE(
        "Destroying command pool {:#x}.",
        reinterpret_cast<uint64_t>(VkCommandPool(_pool))
    );
    LogicalDevice::native().destroyCommandPool(_pool);
}

// =============================================================================
void CmdPool::reset(const vk::CommandPoolResetFlags flags) const {
    LogicalDevice::native().resetCommandPool(_pool, flags);
}

// =============================================================================
CmdPool::CmdPool() :
    _pool { }
{ }

CmdPool::CmdPool(CmdPool &&other) noexcept :
    _pool { other._pool }
{
    other._pool = nullptr;
}

} // namespace vkl