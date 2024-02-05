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
}