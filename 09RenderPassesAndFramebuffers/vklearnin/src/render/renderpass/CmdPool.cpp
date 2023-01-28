#include "vklearnin/vklearnin.hpp"
#include "vklearnin/render/renderpass/CmdPool.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"
#include "vklearnin/system/devices/CmdQueue.hpp"

namespace vkl {

// =============================================================================
void CmdPool::create() {
    const vk::CommandPoolCreateInfo pool_info {
        .queueFamilyIndex = LogicalDevice::cmd_queue().index()
    };

    auto result = LogicalDevice::native().createCommandPool(
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
    for(auto buffer = _buffers.begin(); buffer != _buffers.end(); ++buffer) {
        LogicalDevice::native().freeCommandBuffers(_pool, buffer->native());
    }

    CONSOLE_TRACE(
        "Destroying command pool {:#x}.",
        reinterpret_cast<uint64_t>(VkCommandPool(_pool))
    );
    LogicalDevice::native().destroyCommandPool(_pool);
}

// =============================================================================
CmdPool::BufferIter CmdPool::allocate_buffer() {
    const vk::CommandBufferAllocateInfo buffer_info {
        .commandPool = _pool,
        // A secondary command buffer can be reused between subpasses and even
        // render passes, while a primary command buffer is tied to its pass.
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1u,
    };

    _buffers.resize(_buffers.size() + 1);
    _buffers.back().allocate(buffer_info);
    return std::prev(_buffers.end());
}

// =============================================================================
void CmdPool::reset(const vk::CommandPoolResetFlags flags) const {
    LogicalDevice::native().resetCommandPool(_pool, flags);
}

// =============================================================================
CmdPool::CmdPool() :
    _pool    { },
    _buffers { }
{ }

CmdPool::CmdPool(CmdPool &&other) noexcept :
    _pool    { other._pool },
    _buffers { std::move(other._buffers) }
{
    other._pool = nullptr;
}

} // namespace vkl