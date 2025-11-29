#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"

#include "vklearnin/vulkan/devices/vkDevice.hpp"
#include "vklearnin/vulkan/devices/vkCmdPool.hpp"
#include "vklearnin/vulkan/devices/vkQueue.hpp"

namespace vkl {

// =============================================================================
vkCmdBuffer::vkCmdBuffer(vkCmdBuffer &&other) :
    _handle { other._handle },
    _device { other._device },
    _pool   { other._pool },
    _queue  { other._queue }
{
    other._handle = nullptr;
    other._device = nullptr;
    other._pool   = nullptr;
    other._queue  = nullptr;
}

// =============================================================================
bool vkCmdBuffer::allocate(vkDevice const &device, vkCmdPool const &pool,
                           vkQueue const &queue)
{
    if(_handle || _pool || _queue) {
        Log::error("Command buffer {} already allocated.", _handle);
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot allocate command buffer with invalid "
                          "device.");
        return false;
    }

    if(!pool.native()) {
        Log::error("Cannot allocate command buffer with invalid command "
                          "pool.");
        return false;
    }

    if(!queue.native()) {
        Log::error("Cannot allocate command buffer with invalid device "
                          "queue.");
        return false;
    }

    _device = device.native();
    _pool   = pool.native();
    _queue  = queue.native();

    vk::CommandBufferAllocateInfo const buffer_info {
        .pNext = nullptr,
        .commandPool = _pool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1u,
    };

    auto const result =
        _device.allocateCommandBuffers(&buffer_info, &_handle);

    if(result != vk::Result::eSuccess) {
        Log::error(
            "Failed to allocate command buffer from pool {}: '{}'",
            buffer_info.commandPool,
            vk::to_string(result)
        );
        return false;
    }

    Log::trace(
        "Allocated command buffer {} from pool {}",
        _handle,
        buffer_info.commandPool
    );

    return true;
}

// =============================================================================
bool vkCmdBuffer::free() {
    if(!_handle || !_pool || !_queue) {
        Log::error("Must allocate command buffer before calling free.");
        return false;
    }

    Log::trace("Freeing command buffer {}", _handle);
    _device.freeCommandBuffers(_pool, { _handle });

    _handle = nullptr;
    _device = nullptr;
    _pool   = nullptr;
    _queue  = nullptr;

    return true;
}

// =============================================================================
bool vkCmdBuffer::begin_one_time_submit() const {
    if(!_handle) {
        Log::error("Must allocate command buffer before beginning one "
                          "time submit.");
        return false;
    }

    static vk::CommandBufferBeginInfo const begin_info {
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
    };

    _handle.begin(begin_info);

    return true;
}

// =============================================================================
void vkCmdBuffer::end_recording() const {
    _handle.end();
}

// =============================================================================
void
vkCmdBuffer::begin_render_pass(vk::RenderPassBeginInfo const &begin_info) const
{
    _handle.beginRenderPass(begin_info, vk::SubpassContents::eInline);
}

// =============================================================================
void vkCmdBuffer::end_render_pass() const {
    _handle.endRenderPass();
}

// =============================================================================
void
vkCmdBuffer::begin_rendering(vk::RenderingInfoKHR const &rendering_info) const
{
    _handle.beginRendering(rendering_info);
}

// =============================================================================
void vkCmdBuffer::end_rendering() const {
    _handle.endRendering();
}

// =============================================================================
void vkCmdBuffer::dispatch(GroupCounts const &group_counts) const {
    _handle.dispatch(group_counts.x, group_counts.y, group_counts.z);
}

} // namespace vkl