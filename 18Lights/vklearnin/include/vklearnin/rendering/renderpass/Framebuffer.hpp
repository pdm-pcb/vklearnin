#ifndef VKLEARNIN_RENDERING_RENDERPASS_FRAMEBUFFER_HPP
#define VKLEARNIN_RENDERING_RENDERPASS_FRAMEBUFFER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class RenderPass;
class CmdBuffer;

class Framebuffer final {
public:
    Framebuffer & create_color_buffer(vk::Extent2D const &extent,
                                      vk::SampleCountFlagBits const &samples);
    Framebuffer & create_depth_buffer(vk::Extent2D const &extent,
                                      vk::SampleCountFlagBits const &samples);
    Framebuffer & add_image_view(vk::ImageView const &view);
    Framebuffer & create_shadow_map(vk::Extent2D const &extent);

    void create(vk::Rect2D const &render_area,
                vk::RenderPass const &render_pass);

    void destroy();

    void transition_color_for_draw(ImageObject const &image, CmdBuffer const &cmd_buffer);
    void transition_depth_for_draw(CmdBuffer const &cmd_buffer);
    void transition_color_for_present(ImageObject const &image, CmdBuffer const &cmd_buffer);

    inline auto const & native()      const { return _framebuffer; }
    inline auto const & render_area() const { return _render_area; }

    inline auto const& color_buffer() const { return _color_buffer; }
    inline auto const& depth_buffer() const { return _depth_buffer; }

    inline auto const& shadow_map() const { return _shadow_map;}

    Framebuffer();
    ~Framebuffer() = default;

    Framebuffer(Framebuffer &&other) noexcept;
    Framebuffer(const Framebuffer &) = delete;

    Framebuffer& operator=(Framebuffer &&) = delete;
    Framebuffer& operator=(const Framebuffer &) = delete;

private:
    std::vector<vk::ImageView> _attachments;

    ImageObject _color_buffer;
    ImageObject _depth_buffer;
    Texture2D   _shadow_map;

    vk::Rect2D _render_area;
    vk::Framebuffer _framebuffer;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_RENDERPASS_FRAMEBUFFER_HPP