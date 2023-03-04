#ifndef VKLEARNIN_RENDERINGRENDERCONFIG_HPP
#define VKLEARNIN_RENDERINGRENDERCONFIG_HPP

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
    static float    window_aspect;

    static uint8_t image_count;
    static bool vsync_on;

    static uint8_t msaa_samples;
    static float anisotropy;

    static constexpr std::array<float, 4>
        CLEAR_COLOR { 0.08f, 0.08f, 0.16f, 1.0f };
};

} // namespace vkl

#endif // VKLEARNIN_RENDERINGRENDERCONFIG_HPP