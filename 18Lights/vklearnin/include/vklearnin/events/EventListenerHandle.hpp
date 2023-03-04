// Credit to kainev for this whole shebang:
// https://codereview.stackexchange.com/questions/252884/

#ifndef VKLEARNIN_EVENTS_EVENTLISTENERHANDLE_HPP
#define VKLEARNIN_EVENTS_EVENTLISTENERHANDLE_HPP

#include "vklearnin/system/pch.hpp"

namespace vkl {

struct EventListenerHandle final {
    uint32_t event_id    = 0u;
    uint32_t listener_id = 0u;
};

} // namespace vkl

#endif // VKLEARNIN_EVENTS_EVENTLISTENERHANDLE_HPP