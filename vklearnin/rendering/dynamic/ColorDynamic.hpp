#ifndef VKLEARNIN_RENDERING_DYNAMIC_COLORDYNAMIC_HPP
#define VKLEARNIN_RENDERING_DYNAMIC_COLORDYNAMIC_HPP

#include "vklearnin/pch.hpp"

namespace vkl {

class vkSurface;
class vkDevice;
class vkCmdBuffer;

class ColorDynamic final {
public:
    ColorDynamic() = default;
    ~ColorDynamic() = default;

    ColorDynamic(ColorDynamic &&) = delete;
    ColorDynamic(ColorDynamic const &) = delete;

    ColorDynamic & operator=(ColorDynamic &&) = delete;
    ColorDynamic & operator=(ColorDynamic const &) = delete;

    void init(vkSurface const &surface,
              std::span<vk::ClearValue const> const clear_values);

    void update_render_area(vkSurface const &surface);

    [[nodiscard]]
    vk::RenderingInfo const & rendering_info(vk::ImageView const &view,
                                             vk::ImageLayout const &layout);

    [[nodiscard]] auto const & color_attachment_formats() const {
        return _color_attachment_formats;
    }

private:
    std::vector<vk::Format> _color_attachment_formats;
    std::vector<vk::RenderingAttachmentInfo> _color_attachments;

    vk::RenderingInfo _rendering_info { };
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DYNAMIC_COLORDYNAMIC_HPP