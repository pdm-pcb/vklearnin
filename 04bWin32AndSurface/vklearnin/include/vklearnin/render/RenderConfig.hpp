#ifndef VKLEARNIN_RENDER_RENDERCONFIG_HPP
#define VKLEARNIN_RENDER_RENDERCONFIG_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct RenderConfig final {
    static uint16_t screen_width;
    static uint16_t screen_height;
    static int16_t  screen_x_offset;
    static int16_t  screen_y_offset;

    static uint16_t window_width;
    static uint16_t window_height;
    static int16_t  window_pos_x;
    static int16_t  window_pos_y;
};

} // namespace vkl

#endif // VKLEARNIN_RENDER_RENDERCONFIG_HPP