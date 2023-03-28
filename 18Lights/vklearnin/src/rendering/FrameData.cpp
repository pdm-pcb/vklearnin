#include "vklearnin/vklearnin.hpp"
#include "vklearnin/rendering/FrameData.hpp"

#include "vklearnin/system/devices/LogicalDevice.hpp"

namespace vkl {

// =============================================================================
void FrameData::wait_on_queue_fence() const {
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
}

// =============================================================================
void FrameData::submit_to_device() const {
    // Once LogicalDevice has acquired an image for us, it'll signal this
    // semaphore
    vk::Semaphore const acquire_complete_sems[] {
        _acquire_complete
    };

    // We want the image to be fully acquired before beginning to write to it,
    // so if the color attachemnt output stage is reached but the above
    // semaphore  hasn't signaled, wait on it.
    vk::PipelineStageFlags const acquire_before_stage =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;

    // The command buffer we're getting ready to submit
    vk::CommandBuffer const command_buffers[] {
        _cmd_buffer.native()
    };

    // When it comes time for the presentation engine to take this image back
    // and show it on the display, we'll need a semaphore for it to wait on
    // in case the command buffer hasn't been completely executed.
    vk::Semaphore const commands_complete_sems[] {
        _commands_complete
    };

    // Bring it all together in one struct
    vk::SubmitInfo const submit_info {
        .waitSemaphoreCount = static_cast<uint32_t>(
            std::size(acquire_complete_sems)
        ),
        .pWaitSemaphores   = acquire_complete_sems,
        .pWaitDstStageMask = &acquire_before_stage,
        .commandBufferCount = static_cast<uint32_t>(
            std::size(command_buffers)
        ),
        .pCommandBuffers   = command_buffers,
        .signalSemaphoreCount = static_cast<uint32_t>(
            std::size(commands_complete_sems)
        ),
        .pSignalSemaphores = commands_complete_sems,
    };

    // Submit this work to the queue
    LogicalDevice::cmd_queue().native().submit(submit_info, _queue_complete);
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
void FrameData::_create_sync_primitives() {
    _acquire_complete  = LogicalDevice::native().createSemaphore({ });
    _commands_complete = LogicalDevice::native().createSemaphore({ });

    vk::FenceCreateInfo const fence_info {
        .flags = vk::FenceCreateFlagBits::eSignaled
    };
    _queue_complete    = LogicalDevice::native().createFence(fence_info);

    CONSOLE_TRACE(
        "\nCreated sync primitives:"
        "\n\tacquire complete semaphore  {:#x}"
        "\n\tcommands complete semaphore {:#x}"
        "\n\tqueue complete fence        {:#x}",
        reinterpret_cast<uint64_t>(VkSemaphore(_acquire_complete)),
        reinterpret_cast<uint64_t>(VkSemaphore(_commands_complete)),
        reinterpret_cast<uint64_t>(VkFence(_queue_complete))
    );
}

// =============================================================================
void FrameData::_destroy_cmd_structures() {
    _cmd_buffer.free();
    _cmd_pool.destroy();
}

// =============================================================================
void FrameData::_destroy_sync_primitives() {
    CONSOLE_TRACE(
        "\nDestroying sync primitives:"
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
FrameData::FrameData() :
    _cmd_pool          { },
    _cmd_buffer        { },
    _acquire_complete  { },
    _commands_complete { },
    _queue_complete    { },
    _image_index       { std::numeric_limits<uint32_t>::max() }
{ }

FrameData::FrameData(FrameData &&other) noexcept :
    _cmd_pool          { std::move(other._cmd_pool) },
    _cmd_buffer        { std::move(other._cmd_buffer) },
    _acquire_complete  { other._acquire_complete },
    _commands_complete { other._commands_complete },
    _queue_complete    { other._queue_complete },
    _image_index       { other._image_index }
{
    other._acquire_complete  = vk::Semaphore { };
    other._commands_complete = vk::Semaphore { };
    other._queue_complete    = vk::Fence     { };
}

} // namespace vkl