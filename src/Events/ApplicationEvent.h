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

        EVENT_TYPE(ApplicationStartEvent)
    };

    class ApplicationResizeEvent : public Event
    {
    public:
        float x, y;
        float width, height;

        ApplicationResizeEvent(float x, float y, float width, float height);
        virtual ~ApplicationResizeEvent() = default;

        EVENT_TYPE(ApplicationResizeEvent)
    };

    class ApplicationCloseEvent : public Event
    {
    public:
        ApplicationCloseEvent();
        virtual ~ApplicationCloseEvent() = default;

        EVENT_TYPE(ApplicationCloseEvent)
    };
}
