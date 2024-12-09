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

    bool create(vkSurface const &surface, vkDevice const &device);
    bool destroy();

    void update_render_area(vkSurface const &surface);

    void begin(std::span<vk::ClearValue const> const clear_values,
               vkCmdBuffer const &cmd_buffer);

private:
    vk::RenderingAttachmentInfoKHR _color_attachment;
    vk::RenderingInfoKHR _rendering_info;

    vk::Rect2D _render_area { };
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_DYNAMIC_COLORDYNAMIC_HPP