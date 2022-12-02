#ifndef VKLEARNIN_SYSTEM_EVENTS_EVENTBROKER_HPP
#define VKLEARNIN_SYSTEM_EVENTS_EVENTBROKER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/system/events/Events.hpp"
#include "vklearnin/system/events/EventCallbacks.hpp"

namespace vkl {

class EventBroker {
using CallbackLists = std::vector<EventCallbacksBase *>;
public:
    template<typename Event, typename Handler, typename Callback>
    static EventListenerHandle
    subscribe(Handler *handler, Callback callback) {
        return static_cast<EventCallbacks<Event> *>(
            _callbacks[_event_id<Event>()])->add(
            [handler, callback](const Event &event) {
                (handler->*callback)(event);
            }
        );
    }

    template<typename Event, typename... EventParams>
    static void emit(EventParams ...event_args) {
        Event event { event_args... };
        static_cast<EventCallbacks<Event> *>(
            _callbacks[_event_id<Event>()]
        )->emit(event);
    }

    static void unsubscribe(const EventListenerHandle &handle);

    static void init();
    static void shutdown();

    EventBroker() = delete;
    ~EventBroker() = delete;

    EventBroker(EventBroker &&other) = delete;
    EventBroker(const EventBroker &other) = delete;

    EventBroker & operator=(EventBroker &&other) = delete;
    EventBroker & operator=(const EventBroker &other) = delete;

private:
    static CallbackLists _callbacks;
    static uint32_t _next_event_id;

    template<typename Event>
    static uint32_t _event_id() {
        static uint32_t event_id = _register_event<Event>();
        return event_id;
    }

    template<typename Event>
    static uint32_t _register_event() {
        _callbacks.emplace_back(new EventCallbacks<Event>(_next_event_id));
        return _next_event_id++;
    }
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_EVENTS_EVENTBROKER_HPP