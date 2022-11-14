#ifndef VKLEARNIN_SYSTEM_RENDERCONFIG_HPP
#define VKLEARNIN_SYSTEM_RENDERCONFIG_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct RenderConfig {
    static uint16_t screen_width;
    static uint16_t screen_height;
    static uint16_t screen_x_offset;
    static uint16_t screen_y_offset;

    static uint16_t window_width;
    static uint16_t window_height;
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_RENDERCONFIG_HPP