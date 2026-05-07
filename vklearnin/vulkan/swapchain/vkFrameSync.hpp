#ifndef VKLEARNIN_VULKAN_SWAPCHAIN_VKFRAMESYNC_HPP
#define VKLEARNIN_VULKAN_SWAPCHAIN_VKFRAMESYNC_HPP

#include "vklearnin/pch.hpp"

#include "vklearnin/vulkan/devices/vkCmdPool.hpp"
#include "vklearnin/vulkan/devices/vkCmdBuffer.hpp"

namespace vkl {

class vkDevice;

class vkFrameSync final {
 public:
    vkFrameSync() = default;
    ~vkFrameSync() = default;

    vkFrameSync(vkFrameSync &&other);
    vkFrameSync(const vkFrameSync &) = delete;

    vkFrameSync & operator=(vkFrameSync &&) = delete;
    vkFrameSync & operator=(const vkFrameSync &) = delete;

    bool create(vkDevice const &device);
    bool destroy();

    bool wait_and_reset() const;

    [[nodiscard]] inline auto const & in_flight_fence() const { return _in_flight_fence; }
    [[nodiscard]] inline auto const & wait_semaphore() const { return _wait_sem; }
    [[nodiscard]] inline auto const & complete_semaphore() const { return _complete_sem; }
    [[nodiscard]] inline auto const & cmd_buffer() const { return _cmd_buffer; }

private:
    vk::Fence     _in_flight_fence { nullptr };
    vk::Semaphore _wait_sem        { nullptr };
    vk::Semaphore _complete_sem    { nullptr };

    vkCmdPool   _cmd_pool;
    vkCmdBuffer _cmd_buffer;

    vk::Device _device { nullptr };
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_SWAPCHAIN_VKFRAMESYNC_HPP