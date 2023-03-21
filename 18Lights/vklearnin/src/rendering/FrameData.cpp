#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/FrameData.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void FrameData::wait_on_queue_fence() {
    vk::Fence const queue_fences[] { _queue_complete };
    auto const wait_result = LogicalDevice::native().waitForFences(
        queue_fences,
        VK_TRUE,
        std::numeric_limits<uint64_t>::max()
    );

    if(wait_result != vk::Result::eSuccess) {
        CONSOLE_CRITICAL(
            "Failed to wait on frame queue fence: '{}'",
            to_string(wait_result)
        );
        return;
    }

    LogicalDevice::native().resetFences(queue_fences);

    _cmd_pool.reset();
}

// =============================================================================
void FrameData::init() {
    _create_cmd_structures();
    _create_sync_primitives();
}

// =============================================================================
void FrameData::shutdown() {
    _destroy_sync_primitives();
    _destroy_cmd_structures();
}

// =============================================================================
void FrameData::_create_cmd_structures() {
    _cmd_pool.create();
    _cmd_buffer.allocate(_cmd_pool.native());
}

// =============================================================================
void FrameData::_destroy_cmd_structures() {
    _cmd_buffer.free();
    _cmd_pool.destroy();
}

// =============================================================================
void FrameData::_destroy_sync_primitives() {
    CONSOLE_TRACE(
        "Destroying swapchain sync primitives:"
        "\n\tacquire complete semaphore  {:#x}"
        "\n\tcommands complete semaphore {:#x}"
        "\n\tqueue complete fence        {:#x}",
        reinterpret_cast<uint64_t>(VkSemaphore(_acquire_complete)),
        reinterpret_cast<uint64_t>(VkSemaphore(_commands_complete)),
        reinterpret_cast<uint64_t>(VkFence(_queue_complete))
    );

    LogicalDevice::native().destroySemaphore(_acquire_complete);
    LogicalDevice::native().destroySemaphore(_commands_complete);
    LogicalDevice::native().destroyFence(_queue_complete);
}

// =============================================================================
void FrameData::_create_sync_primitives() {
    _acquire_complete  = LogicalDevice::native().createSemaphore({ });
    _commands_complete = LogicalDevice::native().createSemaphore({ });

    vk::FenceCreateInfo fence_info {
        .flags = vk::FenceCreateFlagBits::eSignaled
    };
    _queue_complete    = LogicalDevice::native().createFence(fence_info);

    CONSOLE_TRACE(
        "Created swapchain sync primitives:"
        "\n\tacquire complete semaphore  {:#x}"
        "\n\tcommands complete semaphore {:#x}"
        "\n\tqueue complete fence        {:#x}",
        reinterpret_cast<uint64_t>(VkSemaphore(_acquire_complete)),
        reinterpret_cast<uint64_t>(VkSemaphore(_commands_complete)),
        reinterpret_cast<uint64_t>(VkFence(_queue_complete))
    );
}

// =============================================================================
FrameData::FrameData() :
    _cmd_pool          { },
    _cmd_buffer        { },
    _acquire_complete  { },
    _commands_complete { },
    _queue_complete    { }
{ }

FrameData::FrameData(FrameData &&other) noexcept :
    _cmd_pool          { std::move(other._cmd_pool) },
    _cmd_buffer        { std::move(other._cmd_buffer) },
    _acquire_complete  { other._acquire_complete },
    _commands_complete { other._commands_complete },
    _queue_complete    { other._queue_complete }
{
    other._acquire_complete  = vk::Semaphore { };
    other._commands_complete = vk::Semaphore { };
    other._queue_complete    = vk::Fence     { };
}

} // namespace vkl