#include "vklearnin/common.hpp"
#include "vklearnin/CommandStructures/SingleUseCommandBuffer.hpp"

#include "vklearnin/Instance.hpp"

// =============================================================================
::VkCommandBuffer & SingleUseCommandBuffer::init()
{
    CONSOLE_INFO("");

    ::VkCommandBufferAllocateInfo alloc_info { };
    alloc_info.sType = ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = ::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = _pool;
    alloc_info.commandBufferCount = 1u;

    auto result = ::vkAllocateCommandBuffers(
        _instance.logical_device(),
        &alloc_info,
        &_buffer
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Failed to allocate command buffer.");
    }

    return _buffer;
}

// =============================================================================
void SingleUseCommandBuffer::begin() {
    CONSOLE_INFO("");

    ::VkCommandBufferBeginInfo buffer_info { };
    buffer_info.sType = ::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    buffer_info.flags = ::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    auto result = ::vkBeginCommandBuffer(_buffer, &buffer_info);

    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Unable to begin command buffer recording.");
    }
}

// =============================================================================
void SingleUseCommandBuffer::end() {
    CONSOLE_INFO("");

    auto result = ::vkEndCommandBuffer(_buffer);
    if(result != ::VK_SUCCESS) {
        CONSOLE_CRITICAL("Failed to record to command buffer.");
    }
}

// =============================================================================
void SingleUseCommandBuffer::submit(const ::VkQueue &queue) {
    CONSOLE_INFO("");

    ::VkSubmitInfo submitInfo{};
    submitInfo.sType = ::VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1u;
    submitInfo.pCommandBuffers = &_buffer;

    auto result = ::vkQueueSubmit(
        queue,
        1u,
        &submitInfo,
        0u
    );

    if(result != ::VK_SUCCESS) {
        CONSOLE_ERROR("Unable to submit buffer command queue.");
    }

    // give it some time...
    ::vkQueueWaitIdle(queue);

    // and we're done
    ::vkFreeCommandBuffers(
        _instance.logical_device(),
        _pool,
        1u,
        &_buffer
    );
}

// =============================================================================
SingleUseCommandBuffer::SingleUseCommandBuffer(const ::VkCommandPool &pool,
                                               const Instance &instance) :
    _pool     { pool },
    _buffer   { nullptr },
    _instance { instance }
{
    CONSOLE_INFO("");
}