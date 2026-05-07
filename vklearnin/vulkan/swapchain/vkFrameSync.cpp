#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/swapchain/vkFrameSync.hpp"

#include "vklearnin/vulkan/devices/vkCmdPool.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"

namespace vkl {

// =============================================================================
vkFrameSync::vkFrameSync(vkFrameSync &&other) :
    _in_flight_fence { other._in_flight_fence },
    _wait_sem        { other._wait_sem },
    _complete_sem    { other._complete_sem },
    _cmd_pool        { std::move(other._cmd_pool) },
    _cmd_buffer      { std::move(other._cmd_buffer) },
    _device          { other._device }
{
    other._in_flight_fence = nullptr;
    other._wait_sem        = nullptr;
    other._complete_sem    = nullptr;
    other._device          = nullptr;
}

// =============================================================================
bool vkFrameSync::create(vkDevice const &device) {
    if(_in_flight_fence
       || _wait_sem
       || _complete_sem)
    {
        Log::error("Frame sync primatives already created.");
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create frame sync with invalid device.");
        return false;
    }

    _device = device.native();

    _cmd_pool.create(
        device,
        vk::CommandPoolCreateInfo {
            .flags = vk::CommandPoolCreateFlagBits::eTransient,
            .queueFamilyIndex = device.graphics_queue().family_index(),
        }
    );

    _cmd_buffer.allocate(device, _cmd_pool, device.graphics_queue());

    vk::FenceCreateInfo const fence_info {
        .pNext = nullptr,
        .flags = vk::FenceCreateFlagBits::eSignaled
    };

    _in_flight_fence = _device.createFence(fence_info);
    _wait_sem        = _device.createSemaphore(vk::SemaphoreCreateInfo { });
    _complete_sem    = _device.createSemaphore(vk::SemaphoreCreateInfo { });

    Log::trace("\nCreated frame sync primitives:"
              "\n    device queue fence         {}"
              "\n    present complete semaphore {}"
              "\n    queue complete semaphore   {}",
              _in_flight_fence,
              _wait_sem,
              _complete_sem);

    return true;
}

// =============================================================================
bool vkFrameSync::destroy() {
    Log::trace("\nDestroying frame sync primitives:"
              "\n    device queue fence         {}"
              "\n    present complete semaphore {}"
              "\n    queue complete semaphore   {}",
              _in_flight_fence,
              _wait_sem,
              _complete_sem);

    _device.destroyFence(_in_flight_fence);
    _device.destroySemaphore(_wait_sem);
    _device.destroySemaphore(_complete_sem);

    _in_flight_fence = nullptr;
    _wait_sem = nullptr;
    _complete_sem   = nullptr;

    _cmd_buffer.free();
    _cmd_pool.destroy();

    return true;
}

// =============================================================================
bool vkFrameSync::wait_and_reset() const {
    if(!_device) {
        Log::error("Must create frame sync before calling wait and reset.");
        return false;
    }

    // Wait for no more than one second
    using namespace std::chrono_literals;
    auto constexpr wait_period =
        std::chrono::duration_cast<std::chrono::nanoseconds>(1.0s).count();

    auto const result = _device.waitForFences(
        _in_flight_fence, // The fence(s) to wait on
        vk::True,         // Whether or not to wait on all provided fences
        wait_period       // How long to wait for these fences
    );

    if(result != vk::Result::eSuccess) {
        Log::error("Failed to wait on queue fence: '{}'",
                   vk::to_string(result));
        return false;
    }

    _device.resetFences(_in_flight_fence);
    _device.resetCommandPool(_cmd_pool.native());

    return true;
}

} // namespace vkl