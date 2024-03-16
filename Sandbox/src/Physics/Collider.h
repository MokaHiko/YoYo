#pragma once

#include "ECS/System.h"
#include "PhysicsTypes.h"

namespace psx
{
    class PhysicsWorld;
    class BoxColliderSystem : public System<BoxColliderComponent>
    {
    public:
        BoxColliderSystem(Scene* scene, PhysicsWorld& world);
        virtual ~BoxColliderSystem();

        virtual void OnComponentCreated(Entity e, BoxColliderComponent& box_collider) override;
        virtual void OnComponentDestroyed(Entity e, BoxColliderComponent& box_collider) override;
    private:
        PhysicsWorld& m_world;
    };
}