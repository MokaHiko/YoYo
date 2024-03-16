#include <Events/Event.h>
#include "ECS/Entity.h"

#pragma once

// Event called when a new entity is focused in the editor
class FocusEntityEvent : public yoyo::Event
{
public:
    EVENT_TYPE(FocusEntityEvent)

    FocusEntityEvent(Entity e)
        :entity(e) {};
    virtual ~FocusEntityEvent() = default;

    Entity entity;
};
