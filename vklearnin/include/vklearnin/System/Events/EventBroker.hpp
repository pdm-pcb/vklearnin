#ifndef VKLEARNIN_SYSTEM_EVENTS_EVENTBROKER_HPP
#define VKLEARNIN_SYSTEM_EVENTS_EVENTBROKER_HPP

#include "vklearnin/pch.hpp"
#include "vklearnin/System/Events/EventCallbackSet.hpp"

#include "vklearnin/System/Events/KeyboardEvent.hpp"

class EventBroker {
using CallbackSets = std::vector<EventCallbackBase *>;
public:
    template<typename Event, typename Callback>
    static EventListener & subscribe(Callback callback) {
        return static_cast<EventCallbackSet<Event> *>(
            _callbacks[_event_id<Event>()])->add(callback);
    }

    template<typename Event, typename Handler, typename Callback>
    static EventListener & subscribe(Handler *handler, Callback callback) {
        return static_cast<EventCallbackSet<Event> *>(
            _callbacks[_event_id<Event>()])->add(
            [handler, callback](const Event &event) {
                (handler->*callback)(event);
            }
        );
    }

    template<typename Event, typename... EventParams>
    static void emit(EventParams ...event_args) {
        Event event { event_args... };
        const auto &callbacks = 
            static_cast<EventCallbackSet<Event> *>(
                _callbacks[_event_id<Event>()])->callbacks();
        for(const auto &callback : callbacks) {
            callback(event);
        }
    }

    static void unsubscribe(const EventListener &handle);

    static void init();
    static void shutdown();

    EventBroker() = delete;
    ~EventBroker() = delete;

    EventBroker(EventBroker &&other) = delete;
    EventBroker(const EventBroker &other) = delete;

    EventBroker & operator=(EventBroker &&other) = delete;
    EventBroker & operator=(const EventBroker &other) = delete;

private:
    static CallbackSets _callbacks;
    static uint32_t     _next_event_id;

    template<typename Event>
    static uint32_t _event_id() {
        static uint32_t event_id = _register_event<Event>();
        return event_id;
    }

    template<typename Event>
    static uint32_t _register_event() {
        _callbacks.emplace_back(new EventCallbackSet<Event>(_next_event_id));
        return _next_event_id++;
    }
};

#endif // VKLEARNIN_SYSTEM_EVENTS_EVENTBROKER_HPP