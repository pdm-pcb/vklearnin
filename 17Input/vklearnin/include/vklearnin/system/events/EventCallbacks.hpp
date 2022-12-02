#ifndef VKLEARNIN_SYSTEM_EVENTS_EVENTCALLBACKS_HPP
#define VKLEARNIN_SYSTEM_EVENTS_EVENTCALLBACKS_HPP

#include "vklearnin/system/pch.hpp"
#include "vklearnin/system/events/EventListenerHandle.hpp"

namespace vkl {

class EventCallbacksBase {
public:
    static constexpr uint32_t MAX_CALLBACKS = 1024u;

    virtual void remove(const EventListenerHandle &) { }

    virtual ~EventCallbacksBase() = default;

    EventCallbacksBase(EventCallbacksBase &&other) = delete;
    EventCallbacksBase(const EventCallbacksBase &other) = delete;

    EventCallbacksBase & operator=(EventCallbacksBase &&other) = delete;
    EventCallbacksBase & operator=(const EventCallbacksBase &other) = delete;

protected:
    EventCallbacksBase() = default;
};

template<typename Event>
class EventCallbacks final : public EventCallbacksBase {

using CallbackList = std::list<std::function<void(const Event &)>>;
using CallbackIter = CallbackList::iterator;
using HandleMap    = std::unordered_map<uint32_t, CallbackIter>; 

public:
    template<typename Callback>
    EventListenerHandle add(Callback callback) {
        _callbacks.emplace_back(callback);
        _handles.emplace(++_next_listener_id, std::prev(_callbacks.end()));
        return {
            .event_id = _event_id,
            .listener_id = _next_listener_id,
        };
    }

    void emit(const Event &event) {
        for(const auto &callback : _callbacks) {
            callback(event);
        }
    }

    void remove(const EventListenerHandle &handle) override {
        auto iter = _handles[handle.listener_id];
        _handles.erase(handle.listener_id);
        _callbacks.erase(iter);
    }

    explicit EventCallbacks(const uint32_t event_id) :
        _event_id { event_id },
        _next_listener_id { 0u }
    { }
    EventCallbacks() = delete;

    ~EventCallbacks() override = default;

    EventCallbacks(EventCallbacks &&other) = delete;
    EventCallbacks(const EventCallbacks &other) = delete;

    EventCallbacks & operator=(EventCallbacks &&other) = delete;
    EventCallbacks & operator=(const EventCallbacks &other) = delete;

private:
    const uint32_t _event_id;
    uint32_t       _next_listener_id;
    CallbackList   _callbacks;
    HandleMap      _handles;
};

} // namespace vkl

#endif // VKLEARNIN_SYSTEM_EVENTS_EVENTCALLBACKS_HPP