#ifndef VKLEARNIN_RENDERING_RENDERCONFIG_HPP
#define VKLEARNIN_RENDERING_RENDERCONFIG_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct RenderConfig {
    static uint16_t screen_width;
    static uint16_t screen_height;
    static uint16_t screen_x_offset;
    static uint16_t screen_y_offset;

    static uint16_t window_width;
    static uint16_t window_height;

    static float fov_deg;
    static float fov_rad;
    static float aspect_ratio;

    static constexpr uint8_t swapchain_image_count = 2;
    static bool vsync_on;

    static float anisotropy;
};

} // namespace vkl

#endif // VKLEARNIN_RENDERING_RENDERCONFIG_HPP