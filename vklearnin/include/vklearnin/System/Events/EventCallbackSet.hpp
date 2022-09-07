#ifndef VKLEARNIN_SYSTEM_EVENTS_EVENTCALLBACKSET_HPP
#define VKLEARNIN_SYSTEM_EVENTS_EVENTCALLBACKSET_HPP

#include "vklearnin/System/Events/EventListener.hpp"

#include <vector>
#include <deque>
#include <functional>

class EventCallbackBase {
public:
    static constexpr uint32_t MAX_CALLBACKS = 1024u;

    virtual void remove(const EventListener::UID uid) { }

    virtual ~EventCallbackBase() = default;

    EventCallbackBase(EventCallbackBase &&other) = delete;
    EventCallbackBase(const EventCallbackBase &other) = delete;

    EventCallbackBase & operator=(EventCallbackBase &&other) = delete;
    EventCallbackBase & operator=(const EventCallbackBase &other) = delete;

protected:
    EventCallbackBase() = default;
};

template<typename Event>
class EventCallbackSet final : public EventCallbackBase {

using CallbackVec = std::vector<std::function<void(const Event &)>>;

public:
    template<typename Callback>
    EventListener & add(Callback callback) {
        auto handle_count = static_cast<uint32_t>(_active_handles.size());

        if(_inactive_handles.size() == 0) {
            _active_handles.emplace_back(handle_count, _event_id);
            _handle_indices[_active_handles.back().index()] = handle_count;
        }
        else {
            auto index = _inactive_handles.front().index();
            _handle_indices[index] = handle_count;
            _active_handles.emplace_back(std::move(_inactive_handles.front()));
            _inactive_handles.pop_front();
        }

        _callbacks.emplace_back(callback);

        return _active_handles.back();
    }

    void remove(const EventListener::UID uid) override {
            auto index_to_remove        = EventListener::index(uid);
            auto active_index_to_remove = _handle_indices[index_to_remove];
            auto active_index_to_swap   = _active_handles.size() - 1;

            if(active_index_to_remove == EventListener::BAD_UID) {
                CONSOLE_CRITICAL("Trying to remove an invalid event listener");
            }

            auto &handle_to_remove = _active_handles[active_index_to_remove];
            auto &handle_to_swap   = _active_handles[active_index_to_swap];

            _handle_indices[handle_to_swap.index()]   = active_index_to_remove;
            _handle_indices[handle_to_remove.index()] = EventListener::BAD_UID;

            handle_to_remove.increment_revision();
            _inactive_handles.push_back(std::move(handle_to_remove));

            std::swap(_active_handles[active_index_to_swap],
                      _active_handles[active_index_to_remove]);

            _active_handles.pop_back();

            std::swap(_callbacks[active_index_to_swap],
                      _callbacks[active_index_to_remove]);

            _callbacks.pop_back();
    }

    inline const CallbackVec & callbacks() const { return _callbacks; }

    explicit EventCallbackSet(const EventListener::EventID event_id) :
        _event_id { event_id }
    {
        _handle_indices.resize(EventListener::MAX_HANDLES,
                               EventListener::BAD_UID);
        _active_handles.reserve(EventListener::MAX_HANDLES);
    }
    EventCallbackSet() = delete;

    ~EventCallbackSet() override = default;

    EventCallbackSet(EventCallbackSet &&other) = delete;
    EventCallbackSet(const EventCallbackSet &other) = delete;

    EventCallbackSet & operator=(EventCallbackSet &&other) = delete;
    EventCallbackSet & operator=(const EventCallbackSet &other) = delete;

private:
    const EventListener::EventID _event_id;
    CallbackVec _callbacks;

    std::vector<EventListener> _active_handles;
    std::deque<EventListener> _inactive_handles;

    std::vector<EventListener::UID> _handle_indices;
};

#endif // VKLEARNIN_SYSTEM_EVENTS_EVENTCALLBACKSET_HPP