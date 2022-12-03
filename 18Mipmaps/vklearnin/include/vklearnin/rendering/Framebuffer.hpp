#ifndef VKLEARNIN_RENDERING_FRAMEBUFFER_HPP
#define VKLEARNIN_RENDERING_FRAMEBUFFER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Swapchain;
class Pipeline;

class Framebuffer final {
public:
    void create(const vk::Extent2D &extent,
                const vk::ImageView &swapchain_image_view,
                const vk::ImageView &depth_buffer_image_view,
                const vk::RenderPass &render_pass);
    void destroy();

    const auto & native() const { return _framebuffer; }

    Framebuffer() = default;
    ~Framebuffer() = default;

    Framebuffer(Framebuffer &&other);
    Framebuffer(const Framebuffer &other) = delete;

    Framebuffer & operator=(Framebuffer &&other) = delete;
    Framebuffer & operator=(const Framebuffer &other) = delete;

private:
    vk::Framebuffer            _framebuffer;
    std::vector<vk::ImageView> _attachments;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_FRAMEBUFFER_HPP