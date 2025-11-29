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

    void submit(vk::SubmitInfo const &submit_info,
                vk::Fence const &fence) const;

    void submit(vk::SubmitInfo const &submit_info) const;

    bool present(vk::PresentInfoKHR const &present_info) const;

    [[nodiscard]] inline auto const & native() const { return _handle;}
    [[nodiscard]] inline auto family_index() const { return _family_index; }

private:
    vk::Queue _handle { nullptr };
    uint32_t _family_index { std::numeric_limits<uint32_t>::max() };
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_DEVICES_VKQUEUE_HPP