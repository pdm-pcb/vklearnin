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

    static uint32_t swapchain_image_count;
    static bool vsync_on;

    static uint32_t max_msaa_samples;
    static float anisotropy;

    static constexpr std::array<float, 4>
        CLEAR_COLOR { 0.08f, 0.08f, 0.16f, 1.0f };

    inline static auto max_msaa_flag() {
        switch(max_msaa_samples) {
            case 64u: return vk::SampleCountFlagBits::e64; break;
            case 32u: return vk::SampleCountFlagBits::e32; break;
            case 16u: return vk::SampleCountFlagBits::e16; break;
            case 8u:  return vk::SampleCountFlagBits::e8;  break;
            case 4u:  return vk::SampleCountFlagBits::e4;  break;
            case 2u:  return vk::SampleCountFlagBits::e2;  break;
            case 1u:  return vk::SampleCountFlagBits::e1;  break;
        }

        CONSOLE_CRITICAL(
            "Unsupported MSAA sample count {}.",
            RenderConfig::max_msaa_samples
        );

        return vk::SampleCountFlagBits::e1;
    }
};

} // namespace vkl

#endif // VKLEARNIN_RENDERINGRENDERCONFIG_HPP