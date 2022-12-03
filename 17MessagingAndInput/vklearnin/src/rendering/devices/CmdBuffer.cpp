#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/devices/CmdBuffer.hpp"

#include "vklearnin/rendering/devices/LogicalDevice.hpp"
#include "vklearnin/rendering/devices/CmdPool.hpp"

namespace vkl {

void CmdBuffer::create(const CmdPool &pool) {
    _pool = pool.native(); // Save for destruction when we need it

    vk::CommandBufferAllocateInfo buffer_info {
        .commandPool = _pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1u,
    };

    auto result = LogicalDevice::native().allocateCommandBuffers(
        &buffer_info,
        &_buffer
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Unable to allocate command buffer");
    }
}

void CmdBuffer::destroy() {
    LogicalDevice::native().freeCommandBuffers(_pool, _buffer);
}

CmdBuffer::CmdBuffer(CmdBuffer &&other) :
    _buffer { std::move(other._buffer) },
    _pool   { std::move(other._pool) }
{ }

} // namespace vkl