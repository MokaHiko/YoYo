#pragma once

#include "Event.h"

namespace yoyo
{
    class ApplicationStartEvent : public Event
    {
    public:
        float app_start_time;

        ApplicationStartEvent();
        virtual ~ApplicationStartEvent() = default;

        const EventType Type() const override {return s_event_type;}
    private:
        static const EventType s_event_type;
    };

    class ApplicationResizeEvent : public Event
    {
    public:
        float x, y;
        float width, height;

        ApplicationResizeEvent(float x, float y, float width, float height);
        virtual ~ApplicationResizeEvent() = default;

        const EventType Type() const override {return s_event_type;}
        static const EventType s_event_type;
    };

    class ApplicationCloseEvent : public Event
    {
    public:
        ApplicationCloseEvent();
        virtual ~ApplicationCloseEvent() = default;

        const EventType Type() const override {return s_event_type;}
        static const EventType s_event_type;
    };
}
