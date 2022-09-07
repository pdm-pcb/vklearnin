#include "vklearnin/common.hpp"
#include "vklearnin/System/Events/EventBroker.hpp"

EventBroker::CallbackSets EventBroker::_callbacks;
EventListener::EventID   EventBroker::_next_event_id;

void EventBroker::unsubscribe(const EventListener &handle) {
    _callbacks[handle.event_id()]->remove(handle.uid());
}

void EventBroker::init() {
    _callbacks.reserve(EventCallbackBase::MAX_CALLBACKS);
}

void EventBroker::shutdown() {
    for(const auto *callback : _callbacks) {
        delete callback;
    }
}