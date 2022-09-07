#include "vklearnin/common.hpp"
#include "vklearnin/System/Events/EventBroker.hpp"

EventBroker::CallbackLists EventBroker::_callbacks;
uint32_t EventBroker::_next_event_id;
bool EventBroker::_initialized =  false;

void EventBroker::unsubscribe(const EventListenerHandle &handle) {
    assert(_initialized);
    _callbacks[handle.event_id]->remove(handle);
}

void EventBroker::init() {
    _callbacks.reserve(EventCallbacksBase::MAX_CALLBACKS);
    _initialized = true;
}

void EventBroker::shutdown() {
    assert(_initialized);
    for(const auto *callback : _callbacks) {
        delete callback;
    }
}