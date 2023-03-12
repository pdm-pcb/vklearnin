#ifndef VKLEARNIN_SYSTEM_EVENTS_KEYBOARDEVENTS_HPP
#define VKLEARNIN_SYSTEM_EVENTS_KEYBOARDEVENTS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/system/input/Keycodes.hpp"
#include "vklearnin/system/input/sdl_to_vkl.hpp"

namespace vkl {

struct KeyPressEvent {
    Keycode code;
};

struct KeyReleaseEvent {
    Keycode code;
};

} //namespace vkl

#endif // VKLEARNIN_SYSTEM_EVENTS_KEYBOARDEVENTS_HPP