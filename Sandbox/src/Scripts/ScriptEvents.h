#pragma once

#include <Events/Event.h>

#include "ScriptableEntity.h"

class ScriptCreatedEvent : public yoyo::Event
{
public:
    ScriptCreatedEvent(ScriptableEntity* scriptable);
    virtual ~ScriptCreatedEvent() = default;

    ScriptableEntity* script;
    EVENT_TYPE(ScriptCreatedEvent)
};
