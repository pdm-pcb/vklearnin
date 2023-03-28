#ifndef VKLEARNIN_RENDERINGRENDERPASS_FRAMEBUFFER_HPP
#define VKLEARNIN_RENDERINGRENDERPASS_FRAMEBUFFER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class RenderPass;

class Framebuffer final {
public:
    void create(vk::Rect2D const &render_area,
                std::vector<vk::ImageView> const &attachments,
                vk::RenderPass const &render_pass);
    void destroy();

    inline auto const & native()      const { return _framebuffer; }
    inline auto const & render_area() const { return _render_area; }

    Framebuffer();
    ~Framebuffer() = default;

    Framebuffer(Framebuffer &&other) noexcept;
    Framebuffer(const Framebuffer &) = delete;

    Framebuffer& operator=(Framebuffer &&) = delete;
    Framebuffer& operator=(const Framebuffer &) = delete;

private:
    vk::Framebuffer _framebuffer;
    vk::Rect2D _render_area;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERINGRENDERPASS_FRAMEBUFFER_HPP