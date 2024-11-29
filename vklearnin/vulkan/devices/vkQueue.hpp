#ifndef VKLEARNIN_VULKAN_DEVICES_VKQUEUE_HPP
#define VKLEARNIN_VULKAN_DEVICES_VKQUEUE_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkDevice;
class vkCmdBuffer;
class vkSwapchain;

class vkQueue final {
public:
    vkQueue() = default;
    ~vkQueue() = default;

    vkQueue(vkQueue &&) = delete;
    vkQueue(vkQueue const &) = delete;

    vkQueue& operator=(vkQueue &&) = delete;
    vkQueue& operator=(vkQueue const &) = delete;

    bool set(vkDevice const &device, uint32_t const family_index);
    bool clear();

    using CmdBuffers = std::span<vk::CommandBuffer const>;
    using WaitSemaphores = std::span<vk::Semaphore const>;
    using WaitStageFlags = std::span<vk::PipelineStageFlags const>;
    using SignalSemaphores = std::span<vk::Semaphore const>;

    bool submit(CmdBuffers const cmd_buffers,
                WaitSemaphores const wait_sems,
                WaitStageFlags const wait_stage_flags,
                SignalSemaphores const signal_sems,
                vk::Fence const &in_flight_fence) const;

    bool submit(CmdBuffers const cmd_buffers) const;

    [[nodiscard]] bool present(vkSwapchain const &swapchain,
                               vk::Semaphore const &wait_sem,
                               uint32_t const image_index) const;

    inline auto const & native() const { return _handle;}
    inline auto family_index() const { return _family_index; }

private:
    vk::Queue _handle { nullptr };
    uint32_t _family_index { std::numeric_limits<uint32_t>::max() };
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_DEVICES_VKQUEUE_HPP