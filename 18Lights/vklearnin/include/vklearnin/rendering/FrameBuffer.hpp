#ifndef VKLEARNIN_RENDERING_FRAMEBUFFER_HPP
#define VKLEARNIN_RENDERING_FRAMEBUFFER_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

class RenderPass;
class CmdBuffer;

class FrameBuffer final {
public:
    FrameBuffer & create_color_buffer(vk::Extent2D const &extent,
                                      vk::SampleCountFlagBits const &samples);
    FrameBuffer & create_depth_buffer(vk::Extent2D const &extent,
                                      vk::SampleCountFlagBits const &samples);
    FrameBuffer & add_image_view(vk::ImageView const &view);
    FrameBuffer & create_shadow_map(vk::Extent2D const &extent);

    void create(vk::Rect2D const &render_area);

    void destroy();

    inline auto const & render_area() const { return _render_area; }

    inline auto const& color_buffer() const { return _color_buffer; }
    inline auto const& depth_buffer() const { return _depth_buffer; }

    inline auto const& shadow_map() const { return _shadow_map;}

    FrameBuffer();
    ~FrameBuffer() = default;

    FrameBuffer(FrameBuffer &&other) noexcept;
    FrameBuffer(const FrameBuffer &) = delete;

    FrameBuffer& operator=(FrameBuffer &&) = delete;
    FrameBuffer& operator=(const FrameBuffer &) = delete;

private:
    std::vector<vk::ImageView> _attachments;

    ImageObject _color_buffer;
    ImageObject _depth_buffer;
    Texture2D   _shadow_map;

    vk::Rect2D _render_area;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_FRAMEBUFFER_HPP