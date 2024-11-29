#ifndef VKLEARNIN_VULKAN_SWAPCHAIN_FRAMEBUFFER_HPP
#define VKLEARNIN_VULKAN_SWAPCHAIN_FRAMEBUFFER_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkRenderPass;
class vkDevice;

class vkFrameBuffer final {
public:
    vkFrameBuffer() = default;
    ~vkFrameBuffer() = default;

    vkFrameBuffer(vkFrameBuffer &&other);
    vkFrameBuffer(vkFrameBuffer const &) = delete;

    vkFrameBuffer & operator=(vkFrameBuffer &&) = delete;
    vkFrameBuffer & operator=(vkFrameBuffer const &) = delete;

    bool create(vkRenderPass const &render_pass,
                std::span<vk::ImageView const> const attachments,
                vk::Extent2D const &extent,
                vkDevice const &device);

    bool destroy();

    inline auto const & native() const { return _handle; }

private:
    vk::Framebuffer _handle { nullptr };
    vk::Device      _device { nullptr };
};

} // namespace vkl

#endif // VKLEARNIN_VULKAN_SWAPCHAIN_FRAMEBUFFER_HPP