#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/devices/CmdBuffer.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void CmdBuffer::allocate(const vk::CommandPool pool, const bool primary) {
    _pool = pool;

    const vk::CommandBufferAllocateInfo buffer_info {
        .commandPool = _pool,

        // A secondary command buffer can be reused between subpasses and even
        // render passes, while a primary command buffer is tied to its pass.
        .level = (primary ? vk::CommandBufferLevel::ePrimary :
                            vk::CommandBufferLevel::eSecondary),

        .commandBufferCount = 1u,
    };

    auto const result = LogicalDevice::native().allocateCommandBuffers(
        &buffer_info,
        &_buffer
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to llocated command buffer from pool {:#x}: '{}'",
            reinterpret_cast<uint64_t>(VkCommandPool(buffer_info.commandPool)),
            to_string(result)
        );
    }
    else {
        CONSOLE_TRACE(
            "Allocated command buffer from pool {:#x}",
            reinterpret_cast<uint64_t>(VkCommandPool(buffer_info.commandPool))
        );
    }
}

// =============================================================================
void CmdBuffer::free() {
    LogicalDevice::native().freeCommandBuffers(
        _pool,
        { _buffer }
    );
}

// =============================================================================
CmdBuffer CmdBuffer::begin_one_time_submit() {
    vk::CommandBufferBeginInfo const begin_info {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
    };

    CmdBuffer cmd_buffer;
    cmd_buffer.allocate(LogicalDevice::transient_pool().native());
    auto const result = cmd_buffer.native().begin(&begin_info);
    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Could not begin recording for one time submit command buffer."
        );
    }

    return cmd_buffer;
}

// =============================================================================
void CmdBuffer::end_one_time_submit(CmdBuffer &cmd_buffer) {
    cmd_buffer.native().end();

    const vk::SubmitInfo submit_info{
        .waitSemaphoreCount   = 0u,
        .pWaitSemaphores      = nullptr,
        .pWaitDstStageMask    = { },
        .commandBufferCount   = 1u,
        .pCommandBuffers      = &(cmd_buffer.native()),
        .signalSemaphoreCount = 0u,
        .pSignalSemaphores    = nullptr,
    };

    LogicalDevice::cmd_queue().native().submit(
        submit_info,
        nullptr
    );

    LogicalDevice::native().waitIdle();

    cmd_buffer.free();
}

// =============================================================================
CmdBuffer::CmdBuffer() :
    _pool   { },
    _buffer { }
{ }

CmdBuffer::CmdBuffer(CmdBuffer &&other) noexcept :
    _pool   { other._pool   },
    _buffer { other._buffer }
{
    other._pool   = nullptr;
    other._buffer = nullptr;
}

} // namespace vkl