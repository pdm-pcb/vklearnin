#ifndef VKLEARNIN_SYSTEM_EVENTS_MOUSEEVENTS_HPP
#define VKLEARNIN_SYSTEM_EVENTS_MOUSEEVENTS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/system/input/Keycodes.hpp"

namespace vkl {

struct MouseMoveEvent {
    int32_t x_offset = 0;
    int32_t y_offset = 0;
};

struct MouseButtonPressEvent {
    Keycode code;
};

struct MouseButtonReleaseEvent {
    Keycode code;
};

struct MouseScrollEvent {
    int32_t vert_offset = 0;
    int32_t horiz_offset = 0;
};

} //namespace vkl

#endif // VKLEARNIN_SYSTEM_EVENTS_MOUSEEVENTS_HPP