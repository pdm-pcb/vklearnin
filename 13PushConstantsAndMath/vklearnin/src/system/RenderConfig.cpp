#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/RenderConfig.hpp"

namespace vkl {

uint16_t RenderConfig::screen_width    = 0;
uint16_t RenderConfig::screen_height   = 0;
uint16_t RenderConfig::screen_x_offset = 0;
uint16_t RenderConfig::screen_y_offset = 0;

uint16_t RenderConfig::window_width  = 0;
uint16_t RenderConfig::window_height = 0;

float RenderConfig::fov_deg = 90.0f;
float RenderConfig::fov_rad = RenderConfig::fov_deg * math::pi_over_180;
float RenderConfig::aspect_ratio = 1.7777778f;

bool RenderConfig::vsync_on = true;

} // namespace vkl