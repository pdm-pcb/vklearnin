#include "vklearnin/vklearnin.hpp"
#include "vklearnin/render/RenderConfig.hpp"

namespace vkl {

uint32_t RenderConfig::screen_width    = 0u;
uint32_t RenderConfig::screen_height   = 0u;
int32_t  RenderConfig::screen_x_offset = 0;
int32_t  RenderConfig::screen_y_offset = 0;

uint32_t RenderConfig::window_width  = 0u;
uint32_t RenderConfig::window_height = 0u;
int32_t  RenderConfig::window_pos_x  = 0;
int32_t  RenderConfig::window_pos_y  = 0;

uint8_t RenderConfig::image_count = 2u;
bool    RenderConfig::vsync_on = true;

} // namespace vkl