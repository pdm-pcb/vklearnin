#ifndef VKLEARNIN_SYSTEM_EVENTS_LISTENERHANDLE_HPP
#define VKLEARNIN_SYSTEM_EVENTS_LISTENERHANDLE_HPP

#include <cstdint>
#include <list>

struct EventListenerHandle final {
    uint32_t event_id;
    uint32_t listener_id;
};

#endif // VKLEARNIN_SYSTEM_EVENTS_LISTENERHANDLE_HPP