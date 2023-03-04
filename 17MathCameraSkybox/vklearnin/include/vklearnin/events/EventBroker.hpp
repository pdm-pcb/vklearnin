// Credit to kainev for this whole shebang:
// https://codereview.stackexchange.com/questions/252884/

#ifndef VKLEARNIN_EVENTS_EVENTBROKER_HPP
#define VKLEARNIN_EVENTS_EVENTBROKER_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/events/EventCallbacks.hpp"

namespace vkl {

class EventBroker {
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
    using CallbackLists = std::vector<EventCallbacksBase *>;
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
        CONSOLE_TRACE("Event callbacks registered: {}", _callbacks.size());
        return _next_event_id++;
    }
};

} // namespace vkl

#endif //  VKLEARNIN_EVENTS_EVENTBROKER_HPP