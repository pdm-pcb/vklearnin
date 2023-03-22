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
    LogicalDevice::native().freeCommandBuffers(_pool, { _buffer });
}

// =============================================================================
void CmdBuffer::begin_one_time_submit() const {
    vk::CommandBufferBeginInfo const begin_info {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
    };

    _buffer.begin(begin_info);
}

// =============================================================================
void CmdBuffer::end() const {
    _buffer.end();
}

// =============================================================================
void CmdBuffer::submit_and_wait_device() const {
    vk::SubmitInfo const submit_info {
        .commandBufferCount = 1u,
        .pCommandBuffers = &_buffer
    };
    LogicalDevice::cmd_queue().native().submit(submit_info);
    LogicalDevice::native().waitIdle();
}

// =============================================================================
void CmdBuffer::begin_render_pass(vk::RenderPassBeginInfo const &info) const {
    _buffer.beginRenderPass(info, vk::SubpassContents::eInline);
}

// =============================================================================
void CmdBuffer::end_render_pass() const {
    _buffer.endRenderPass();
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