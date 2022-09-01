#include "vklearnin/common.hpp"
#include "vklearnin/CommandStructures/SingleUseCommandBuffer.hpp"

#include "vklearnin/Instance.hpp"

// =============================================================================
vk::CommandBuffer & SingleUseCommandBuffer::init()
{
    CONSOLE_INFO("");

    vk::CommandBufferAllocateInfo alloc_info {
        .commandPool = _pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1u
    };

    auto result = _instance.logical_device().allocateCommandBuffers(
        &alloc_info,
        &_buffer
    );

    if(result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL("Failed to allocate command buffer.");
    }

    return _buffer;
}

// =============================================================================
void SingleUseCommandBuffer::begin() {
    CONSOLE_INFO("");

    vk::CommandBufferBeginInfo buffer_info {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    _buffer.begin(buffer_info);
}

// =============================================================================
void SingleUseCommandBuffer::end() {
    CONSOLE_INFO("");

     _buffer.end();
}

// =============================================================================
void SingleUseCommandBuffer::submit(const vk::Queue &queue) {
    CONSOLE_INFO("");

    vk::SubmitInfo submitInfo {
        .commandBufferCount = 1u,
        .pCommandBuffers = &_buffer
    };

    queue.submit(submitInfo);

    // give it some time...
    queue.waitIdle();

    // and we're done
    _instance.logical_device().freeCommandBuffers(_pool, _buffer);
}

// =============================================================================
SingleUseCommandBuffer::SingleUseCommandBuffer(const vk::CommandPool &pool,
                                               const Instance &instance) :
    _pool     { pool },
    _buffer   { nullptr },
    _instance { instance }
{
    CONSOLE_INFO("");
}