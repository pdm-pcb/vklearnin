#ifndef VKLEARNIN_SYSTEM_EVENTS_EVENTLISTENERHANDLE_HPP
#define VKLEARNIN_SYSTEM_EVENTS_EVENTLISTENERHANDLE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct EventListenerHandle final {
    uint32_t event_id    = 0u;
    uint32_t listener_id = 0u;
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_EVENTS_EVENTLISTENERHANDLE_HPP