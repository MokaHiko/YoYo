#pragma once

#include "Core/Memory.h"
#include "Platform/Platform.h"

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

#define EVENT_TYPE(type)    const EventType Type() const override {return s_event_type;}\
                            inline static const EventType s_event_type = Platform::GenerateUUIDV4();\

    // An event handler is a funciton that takes in an event and return whether or not an event has been handled
    using EventHandler = std::function<bool(Ref<Event> event)>;

    // Singleton event manager class
    class EventManager
    {
    public:
        static EventManager* Instance()
        {
            static EventManager* event_manager;

            if (!event_manager)
            {
                event_manager = Y_NEW EventManager;
            }

            return event_manager;
        }

        void Subscribe(const EventType& type, const EventHandler& handler);
        void Unsubscribe(const EventType& type, EventHandler& handler);

        // Dispatches an event
        void Dispatch(Ref<Event> event);

        // TODO: Make thread safe event queue for dispatch
        // void QueueEvent(Ref<Event> event);
    private:
        EventManager() = default;
        ~EventManager() = default;

        std::unordered_map<EventType, std::list<EventHandler>> m_event_listeners;
    };
}
