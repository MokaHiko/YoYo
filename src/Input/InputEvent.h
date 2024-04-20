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

        int x; // relative to window
        int y; // relative to window

        EVENT_TYPE(MouseButtonDownEvent)
    };

    class MouseMoveEvent: public Event
    {
    public:
        MouseMoveEvent(int _x, int _y, int _x_rel, int _y_rel);
        virtual ~MouseMoveEvent() = default;

        int x; // relative to window
        int y; // relative to window

        int x_rel; // relative motion in x direction
        int y_rel; // relative motion in y direction

        EVENT_TYPE(MouseMoveEvent)
    };
}
