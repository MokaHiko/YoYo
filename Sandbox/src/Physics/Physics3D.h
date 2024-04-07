#pragma once

#include <Events/Event.h>

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include "ECS/System.h"
#include "PhysicsTypes.h"

namespace psx
{
    class SimulationEventCallback : public physx::PxSimulationEventCallback
    {
    public:
        virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override { PX_UNUSED(constraints); PX_UNUSED(count); }
        virtual void onWake(physx::PxActor** actors, physx::PxU32 count) override { PX_UNUSED(actors); PX_UNUSED(count); }
        virtual void onSleep(physx::PxActor** actors, physx::PxU32 count) override { PX_UNUSED(actors); PX_UNUSED(count); }
        virtual void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32) override {}
        virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;
        virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
    private:
    };

    class BoxColliderSystem;
    class PhysicsWorld : public System<RigidBodyComponent>
    {
    public:
        PhysicsWorld(Scene* scene);
        virtual ~PhysicsWorld() = default;

        virtual void Init() override;
        virtual void Shutdown() override;
        virtual void Update(float dt) override;

        virtual void OnComponentCreated(Entity e, RigidBodyComponent& rb) override;
        virtual void OnComponentDestroyed(Entity e, RigidBodyComponent& rb) override;
    public:
        // Actor functions
        void AttachBoxShape(RigidBodyComponent& rb, const yoyo::Vec3& extents, physx::PxBoxGeometry** box_shape, PhysicsMaterial* material = nullptr);

        // Casts a ray, from point origin, in direction direction, of length maxDistance, against all colliders in the Scene.
        bool Raycast(const yoyo::Vec3& origin, const yoyo::Vec3& dir, float max_distance, RaycastHit& out);
    private:
        physx::PxRigidDynamic* CreateDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxVec3& velocity = physx::PxVec3(0));
        void CreateStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent);
    private:
        // Subsystem
        Ref<BoxColliderSystem> m_box_collider_system;
    private:
        physx::PxDefaultAllocator m_allocator;
        physx::PxDefaultErrorCallback   m_errorCallback;
        physx::PxFoundation* m_foundation;
        physx::PxPhysics* m_physics;
        physx::PxDefaultCpuDispatcher* m_dispatcher;
        physx::PxScene* m_scene;
        physx::PxMaterial* m_material;
        physx::PxPvd* m_pvd;

        SimulationEventCallback* m_simulation_event_callback;
    };
}