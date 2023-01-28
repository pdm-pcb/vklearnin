#ifndef VKLEARNIN_RENDER_RENDERCONFIG_HPP
#define VKLEARNIN_RENDER_RENDERCONFIG_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct RenderConfig final {
    static uint32_t screen_width;
    static uint32_t screen_height;
    static int32_t  screen_x_offset;
    static int32_t  screen_y_offset;

    static uint32_t window_width;
    static uint32_t window_height;
    static int32_t  window_pos_x;
    static int32_t  window_pos_y;

    static uint8_t image_count;
    static bool vsync_on;
};

} // namespace vkl

#endif // VKLEARNIN_RENDER_RENDERCONFIG_HPP