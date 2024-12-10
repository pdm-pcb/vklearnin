#include "vklearnin/vklearnin.hpp"
#include "vklearnin/vulkan/swapchain/vkFrameSync.hpp"

#include "vklearnin/vulkan/devices/vkCmdPool.hpp"
#include "vklearnin/vulkan/devices/vkDevice.hpp"

namespace vkl {

// =============================================================================
vkFrameSync::vkFrameSync(vkFrameSync &&other) :
    _in_flight_fence     { other._in_flight_fence },
    _wait_sem { other._wait_sem },
    _complete_sem   { other._complete_sem },
    _cmd_pool            { std::move(other._cmd_pool) },
    _cmd_buffer          { std::move(other._cmd_buffer) },
    _device              { other._device }
{
    other._in_flight_fence     = nullptr;
    other._wait_sem = nullptr;
    other._complete_sem   = nullptr;
    other._device              = nullptr;
}

// =============================================================================
bool vkFrameSync::create(vkDevice const &device) {
    if(_in_flight_fence || _wait_sem || _complete_sem) {
        Log::error("Frame sync primatives already created.");
        return false;
    }

    if(!device.native()) {
        Log::error("Cannot create frame sync with invalid device.");
        return false;
    }

    _device = device.native();

    _cmd_pool.create(device,
                     device.cmd_queue().family_index(),
                     vk::CommandPoolCreateFlagBits::eTransient);

    _cmd_buffer.allocate(device, _cmd_pool, device.cmd_queue());

    vk::FenceCreateInfo const fence_info {
        .pNext = nullptr,
        .flags = vk::FenceCreateFlagBits::eSignaled
    };

    auto [ fence_result, in_flight_fence ] = _device.createFence(fence_info);

    if(fence_result != vk::Result::eSuccess) {
        Log::error("Failed to create queue fence: '{}'",
                  vk::to_string(fence_result));
        return false;
    }

    _in_flight_fence = in_flight_fence;

    auto [ wait_sem_result, wait_sem ] =
        _device.createSemaphore(vk::SemaphoreCreateInfo { });

    if(wait_sem_result != vk::Result::eSuccess) {
        Log::error("Failed to create present semaphore: '{}'",
                  vk::to_string(wait_sem_result));
        return false;
    }

    _wait_sem = wait_sem;

    auto [ complete_sem_result, complete_sem ] =
        _device.createSemaphore(vk::SemaphoreCreateInfo { });

    if(complete_sem_result != vk::Result::eSuccess) {
        Log::error("Failed to create queue semaphore: '{}'",
                  vk::to_string(complete_sem_result));
        return false;
    }

    _complete_sem = complete_sem;

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
    _cmd_buffer.free();
    _cmd_pool.destroy();

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
        vk::True,          // Whether or not to wait on all provided fences
        wait_period       // How long to wait for these fences
        // std::numeric_limits<uint64_t>::max()
    );

    if(result != vk::Result::eSuccess) {
        Log::error("Failed to wait on queue fence: '{}'",
                  vk::to_string(result));
    }

    _device.resetFences(_in_flight_fence);
    _device.resetCommandPool(_cmd_pool.native());

    return true;
}

} // namespace vkl