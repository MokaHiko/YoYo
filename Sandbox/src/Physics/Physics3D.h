#pragma once

#include "ECS/System.h"
#include "PhysicsTypes.h"

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

namespace psx
{
    class SimulationEventCallback : physx::PxSimulationEventCallback
    {
    public:
        virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
    private:
    };

    class PhysicsWorld : System<RigidBodyComponent>
    {
    public:
        PhysicsWorld(Scene* scene)
            :System<RigidBodyComponent>(scene)
        {
        }

        virtual void Init() override;
        virtual void Shutdown() override;
        virtual void Update(float dt) override;

        virtual void OnComponentCreated(Entity e, RigidBodyComponent& rb) override;
        virtual void OnComponentDestroyed(Entity e, RigidBodyComponent& rb) override;
    private:
        physx::PxRigidDynamic* CreateDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, const physx::PxVec3& velocity = physx::PxVec3(0));
        void CreateStack(const physx::PxTransform& t, physx::PxU32 size, physx::PxReal halfExtent);
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