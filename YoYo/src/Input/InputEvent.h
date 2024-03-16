#pragma once

#include "Events/Event.h"
#include "Input.h"

namespace yoyo
{
    class KeyDownEvent : public Event
    {
    public:
        KeyDownEvent(KeyCode key_pressed);
        virtual ~KeyDownEvent() = default;

        KeyCode key;

        EVENT_TYPE(KeyDownEvent)
    };

    class KeyUpEvent: public Event
    {
    public:
        KeyUpEvent(KeyCode key_released);
        virtual ~KeyUpEvent() = default;

        KeyCode key;

        EVENT_TYPE(KeyUpEvent)
    };

    class MouseButtonUpEvent: public Event
    {
    public:
        MouseButtonUpEvent(int index);
        virtual ~MouseButtonUpEvent() = default;

        int button;

        EVENT_TYPE(MouseButtonUpEvent)
    };

    class MouseButtonDownEvent: public Event
    {
    public:
        MouseButtonDownEvent(int index);
        virtual ~MouseButtonDownEvent() = default;

        int button;

        EVENT_TYPE(MouseButtonDownEvent)
    };
}
