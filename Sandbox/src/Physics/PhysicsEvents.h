#pragma once

#include <Events/Event.h>

#include "PhysicsTypes.h"

namespace psx
{
    class CollisionEvent : public yoyo::Event
    {
    public:
        CollisionEvent(const Collision& col);
        virtual ~CollisionEvent() = default;

        Collision collision;
        EVENT_TYPE(CollisionEvent)
    };
}