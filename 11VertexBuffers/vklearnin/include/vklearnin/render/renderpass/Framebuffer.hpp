#ifndef VKLEARNIN_RENDER_RENDERPASS_FRAMEBUFFER_HPP
#define VKLEARNIN_RENDER_RENDERPASS_FRAMEBUFFER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class RenderPass;

class Framebuffer final {
public:
    void create(const std::vector<vk::ImageView> &attachments,
                const RenderPass &render_pass);
    void destroy();

    inline auto native() const { return _framebuffer; }

    Framebuffer();
    ~Framebuffer() = default;

    Framebuffer(Framebuffer &&other) noexcept;
    Framebuffer(const Framebuffer &) = delete;

    Framebuffer & operator=(Framebuffer &&) = delete;
    Framebuffer & operator=(const Framebuffer &) = delete;

private:
    vk::Framebuffer _framebuffer;
};

} // namespace vkl

#endif // VKLEARNIN_RENDER_RENDERPASS_FRAMEBUFFER_HPP