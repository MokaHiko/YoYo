#pragma once

#include "Core/Memory.h"

namespace yoyo
{
    using EventType = uint32_t;

    // Base event class
    class Event
    {
    public:
        Event() = default;
        virtual ~Event() {};

        virtual const EventType Type() const = 0; // Returns the unique id of the event
    };

    // An event handler is a funciton that takes in an event and return whether or not an event has been handled
    using EventHandler = std::function<bool(Ref<Event> event)>;

    class EventManager
    {
    public:
        void Init();
        void Shutdown();

        void Subscribe(const EventType& type, const EventHandler& handler);
        void Unsubscribe(const EventType& type, EventHandler& handler);

        // Dispatches an event
        void Dispatch(Ref<Event> event);
    private:
        std::unordered_map<EventType, std::list<EventHandler>> m_event_listeners;
    };
}
