#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/devices/CmdPool.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"
#include "vklearnin/rendering/devices/CmdQueue.hpp"

namespace vkl {

void CmdPool::reset(const vk::CommandPoolResetFlags flags) const {
    LogicalDevice::native().resetCommandPool(_pool, flags);
}

void CmdPool::create() {
    vk::CommandPoolCreateInfo pool_info {
        .flags = { },
        .queueFamilyIndex = LogicalDevice::cmd_queue().index()
    };

    auto result = LogicalDevice::native().createCommandPool(
        &pool_info,
        nullptr,
        &_pool
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to create command pool.");
    }
    else {
        CONSOLE_TRACE(
            "Created command pool {:#x}.",
            reinterpret_cast<uint64_t>(VkCommandPool(_pool))
        );
    }
}

void CmdPool::destroy() {
    CONSOLE_TRACE(
        "Destroying command pool {:#x}.",
        reinterpret_cast<uint64_t>(VkCommandPool(_pool))
    );
    LogicalDevice::native().destroyCommandPool(_pool);
}

CmdPool::CmdPool(CmdPool &&other) :
    _pool { std::move(other._pool) }
{ }

} // namespace vkl