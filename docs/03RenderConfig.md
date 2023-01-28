## `RenderConfig`
There will be a collection of values that several parts of the library will care about related directly to rendering. First among them will be related to creating and maintaining our drawing surface. As such, I've thrown together a `RenderConfig` struct and made its member variables `static`. It'll grow as  we add more features, but for now we only need eight.

Oh, and it's a global storage structure with no access controls. Fight me. ;)

```cpp
#ifndef VKLEARNIN_SYSTEM_RENDERCONFIG_HPP
#define VKLEARNIN_SYSTEM_RENDERCONFIG_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct RenderConfig {
    static uint16_t screen_width;
    static uint16_t screen_height;
    static int16_t  screen_x_offset;
    static int16_t  screen_y_offset;

    static uint16_t window_width;
    static uint16_t window_height;
    static int16_t  window_x_pos;
    static int16_t  window_y_pos;
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_RENDERCONFIG_HPP
```

And the obligatory single translation unit definition, with some possibly sensible minimum defaults thrown in:

```cpp
#include "vklearnin/vklearnin.hpp"
#include "vklearnin/system/RenderConfig.hpp"

namespace vkl {

uint16_t RenderConfig::screen_width    = 1280u;
uint16_t RenderConfig::screen_height   = 720u;
int16_t  RenderConfig::screen_x_offset = 0u;
int16_t  RenderConfig::screen_y_offset = 0u;

uint16_t RenderConfig::window_width  = 800u;
uint16_t RenderConfig::window_height = 450u;
int16_t  RenderConfig::window_x_pos  = 10u;
int16_t  RenderConfig::window_y_pos  = 10u;

} // namespace vkl
```

## What's Next?
Any graphical feature needs a surface upon which to be drawn. In Vulkan, we ask the host OS to provide us an extension used to create just such a surface. This was done in the last chapter, during the initialization of `VKInstance`. In order to put that surface extension to use though, there needs to be a platform window available first. Let's make that happen.