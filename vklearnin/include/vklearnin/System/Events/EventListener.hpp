#ifndef VKLEARNIN_SYSTEM_EVENTS_LISTENERHANDLE_HPP
#define VKLEARNIN_SYSTEM_EVENTS_LISTENERHANDLE_HPP

#include <cstdint>

class EventListener final {
public:
    using UID      = uint32_t;
    using EventID  = uint32_t;
    using Index    = uint32_t;
    using Revision = uint32_t;

    static constexpr uint32_t IDX_BITS    = 26u;
    static constexpr uint32_t REV_BITS    = 6u;
    static constexpr uint32_t MAX_HANDLES = 1024u;
    static constexpr uint32_t MAX_REVS    = 1u << REV_BITS;
    static constexpr uint32_t BAD_UID     = static_cast<UID>(-1);

    static Index index(const UID uid) {
        return uid >> REV_BITS;
    };
    static Revision revision(const UID uid) {
        return uid & (BAD_UID >> IDX_BITS);
    };

    inline UID      uid()      const { return _uid; }
    inline EventID  event_id() const { return _event_id; }
    inline Index    index()    const { return _uid >> REV_BITS; }
    inline Revision revision() const { return _uid & (BAD_UID >> IDX_BITS);}

    inline void increment_revision() {
        auto this_index    = index();
        auto this_revision = revision();
        this_revision += 1u;

        _uid = this_index << REV_BITS | this_revision;
    }

    EventListener(const Index index, const EventID event_id) :
        _uid      { index << REV_BITS },
        _event_id { event_id }
    { }

    EventListener(EventListener &&other) :
        _uid { other._uid },
        _event_id { other._event_id }
    {
        other._uid = BAD_UID;
        other._event_id = std::numeric_limits<EventID>::max();
    }

    EventListener(const EventListener &other) :
        _uid { other._uid },
        _event_id { other._event_id }
    { }

    EventListener & operator=(EventListener &&other) {
        _uid = other._uid;
        _event_id = other._event_id;

        other._uid = BAD_UID;
        other._event_id = std::numeric_limits<EventID>::max();

        return *this;
    }

    EventListener & operator=(const EventListener &other) {
        _uid = other._uid;
        _event_id = other._event_id;

        return *this;
    }

private:
    UID     _uid;
    EventID _event_id;
};

#endif // VKLEARNIN_SYSTEM_EVENTS_LISTENERHANDLE_HPP