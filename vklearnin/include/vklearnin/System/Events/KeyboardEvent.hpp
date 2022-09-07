#ifndef VKLEARNIN_SYSTEM_EVENTS_KEYBOARDEVENT_HPP
#define VKLEARNIN_SYSTEM_EVENTS_KEYBOARDEVENT_HPP

#include "vklearnin/pch.hpp"

struct KeyPressEvent {
    uint32_t keycode;
};

struct KeyReleaseEvent {
    uint32_t keycode;
};

#endif // VKLEARNIN_SYSTEM_EVENTS_KEYBOARDEVENT_HPP