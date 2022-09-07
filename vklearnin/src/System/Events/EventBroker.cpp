#include "vklearnin/common.hpp"
#include "vklearnin/System/Events/EventBroker.hpp"

EventBroker::CallbackLists EventBroker::_callbacks;
uint32_t EventBroker::_next_event_id;

void EventBroker::unsubscribe(const EventListenerHandle &handle) {
    _callbacks[handle.event_id]->remove(handle);
}

void EventBroker::init() {
    _callbacks.reserve(EventCallbacksBase::MAX_CALLBACKS);
}

void EventBroker::shutdown() {
    for(const auto *callback : _callbacks) {
        delete callback;
    }
}