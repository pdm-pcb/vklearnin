#include "vklearnin/vklearnin.hpp"
#include "vklearnin/render/renderpass/CmdBuffer.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void CmdBuffer::allocate(const vk::CommandBufferAllocateInfo &buffer_info) {
    auto result = LogicalDevice::native().allocateCommandBuffers(
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
CmdBuffer::CmdBuffer() :
    _buffer { }
{ }

CmdBuffer::CmdBuffer(CmdBuffer &&other) noexcept :
    _buffer { other._buffer }
{
    other._buffer = nullptr;
}

} // namespace vkl