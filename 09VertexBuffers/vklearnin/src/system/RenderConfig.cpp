#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/RenderConfig.hpp"

namespace vkl {

uint16_t RenderConfig::screen_width    = 0;
uint16_t RenderConfig::screen_height   = 0;
uint16_t RenderConfig::screen_x_offset = 0;
uint16_t RenderConfig::screen_y_offset = 0;

uint16_t RenderConfig::window_width  = 0;
uint16_t RenderConfig::window_height = 0;

bool RenderConfig::vsync_on = true;

} // namespace vkl