#ifndef VKLEARNIN_RENDERING_FRAMEBUFFER_HPP
#define VKLEARNIN_RENDERING_FRAMEBUFFER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class Swapchain;
class Pipeline;

class Framebuffer final {
public:
    void create(const vk::Extent2D &extent,
                const std::vector<vk::ImageView> &attachments,
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
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_FRAMEBUFFER_HPP