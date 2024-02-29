#include "Physics3D.h"

#include <Core/Log.h>
#include <Core/Time.h>

#include "ECS/Components/Components.h"

namespace psx
{
	using namespace physx;

	static PxReal stackZ = 10.0f;

	PxRigidDynamic* PhysicsWorld::CreateDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity)
	{
		PxRigidDynamic* dynamic = PxCreateDynamic(*m_physics, t, geometry, *m_material, 10.0f);
		dynamic->setAngularDamping(0.5f);
		dynamic->setLinearVelocity(velocity);
		m_scene->addActor(*dynamic);
		return dynamic;
	}

	void PhysicsWorld::CreateStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
	{
		PxShape* shape = m_physics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *m_material);
		for (PxU32 i = 0; i < size;i++)
		{
			for (PxU32 j = 0;j < size - i;j++)
			{
				PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
				PxRigidDynamic* body = m_physics->createRigidDynamic(t.transform(localTm));
				body->attachShape(*shape);
				PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
				m_scene->addActor(*body);
			}
		}
		shape->release();
	}

	void PhysicsWorld::Init()
	{
		m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback);

		m_pvd = PxCreatePvd(*m_foundation);
		PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		m_pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

		m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale(), true, m_pvd);

		if (!m_physics)
		{
			YFATAL("Failed to create PxPhysics Instance!");
		}

		PxSceneDesc scene_desc(m_physics->getTolerancesScale());
		scene_desc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		m_dispatcher = PxDefaultCpuDispatcherCreate(2);
		scene_desc.cpuDispatcher = m_dispatcher;
		scene_desc.filterShader = PxDefaultSimulationFilterShader;
		m_scene = m_physics->createScene(scene_desc);

		PxPvdSceneClient* pvd_client = m_scene->getScenePvdClient();
		if (pvd_client)
		{
			pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvd_client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}

		m_material = m_physics->createMaterial(0.5f, 0.5f, 0.6f);

		PxRigidStatic* groundPlane = PxCreatePlane(*m_physics, PxPlane(0, 1, 0, 0), *m_material);
		m_scene->addActor(*groundPlane);

		// for (PxU32 i = 0;i < 5;i++)
		// {
		// 	CreateStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);
		// }

		// if (!false)
		// 	CreateDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));
	}

	void PhysicsWorld::Shutdown()
	{
		PX_RELEASE(m_physics);
		PX_RELEASE(m_dispatcher);
		PX_RELEASE(m_physics);

		if (m_pvd)
		{
			PxPvdTransport* transport = m_pvd->getTransport();
			m_pvd->release(); m_pvd = NULL;
			PX_RELEASE(transport);
		}

		PX_RELEASE(m_foundation);
	}

	void PhysicsWorld::Update(float dt)
	{
		{
			yoyo::ScopedTimer profiler([&](const yoyo::ScopedTimer& timer) {
					// YINFO("Physics time %.5fms", timer.delta * 1000.0f);
			});

			m_scene->simulate(1.0f / 60.0f);
			m_scene->fetchResults(true);
		}

		{
			using namespace physx;
			for (auto entity : GetScene()->Registry().group<TransformComponent, RigidBodyComponent>())
			{
				Entity e(entity, GetScene());
				TransformComponent& transform = e.GetComponent<TransformComponent>();
				RigidBodyComponent& rb = e.GetComponent<RigidBodyComponent>();

				PxTransform t = rb.actor->getGlobalPose();
				transform.position = { t.p.x, t.p.y, t.p.z };
				transform.quat_rotation = { t.q.x, t.q.y, t.q.z, t.q.w };
			}
		}
	}

	void PhysicsWorld::OnComponentCreated(Entity e, RigidBodyComponent& rb)
	{
		using namespace physx;

		// TODO: Cache shapes
		float half_extent = 1.0f;
		PxShape* shape = m_physics->createShape(PxBoxGeometry(half_extent, half_extent, half_extent), *m_material);

		const TransformComponent& transform = e.GetComponent<TransformComponent>();

		PxQuat q = { 0, 0, 0, 1 };
		q *= PxGetRotXQuat(yoyo::DegToRad(transform.rotation.x));
		q *= PxGetRotYQuat(yoyo::DegToRad(transform.rotation.y));
		q *= PxGetRotZQuat(yoyo::DegToRad(transform.rotation.z));

		PxTransform t = {transform.position.x, transform.position.y, transform.position.z, q};

		rb.actor = m_physics->createRigidDynamic(t);
		rb.actor->userData = (void*)(e.Id());
		rb.actor->attachShape(*shape);
		m_scene->addActor(*rb.actor);

		shape->release();
	}

	void PhysicsWorld::OnComponentDestroyed(Entity e, RigidBodyComponent& rb)
	{
		using namespace physx;
		const TransformComponent& transform = e.GetComponent<TransformComponent>();
		PxTransform t = { transform.position.x, transform.position.y, transform.position.z };

		// Implicit release of shape
		rb.actor->release();
	}

	void SimulationEventCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
	{
		// // Retrieve the current poses and velocities of the two actors involved in the contact event.
		// {
		// 	const PxTransform body0PoseAtEndOfSimulateStep = pairHeader.actors[0]->getGlobalPose();
		// 	const PxTransform body1PoseAtEndOfSimulateStep = pairHeader.actors[1]->getGlobalPose();

		// 	const PxVec3 body0LinVelAtEndOfSimulateStep = pairHeader.actors[0]->is<PxRigidDynamic>() ? pairHeader.actors[0]->is<PxRigidDynamic>()->getLinearVelocity() : PxVec3(PxZero);
		// 	const PxVec3 body1LinVelAtEndOfSimulateStep = pairHeader.actors[0]->is<PxRigidDynamic>() ? pairHeader.actors[1]->is<PxRigidDynamic>()->getLinearVelocity() : PxVec3(PxZero);

		// 	const PxVec3 body0AngVelAtEndOfSimulateStep = pairHeader.actors[0]->is<PxRigidDynamic>() ? pairHeader.actors[0]->is<PxRigidDynamic>()->getAngularVelocity() : PxVec3(PxZero);
		// 	const PxVec3 body1AngVelAtEndOfSimulateStep = pairHeader.actors[0]->is<PxRigidDynamic>() ? pairHeader.actors[1]->is<PxRigidDynamic>()->getAngularVelocity() : PxVec3(PxZero);
		// }

		// // Retrieve the poses and velocities of the two actors involved in the contact event as they were
		// // when the contact event was detected.

		// PxContactPairExtraDataIterator iter(pairHeader.extraDataStream, pairHeader.extraDataStreamSize);
		// while (iter.nextItemSet())
		// {
		// 	const PxTransform body0PoseAtContactEvent = iter.eventPose->globalPose[0];
		// 	const PxTransform body1PoseAtContactEvent = iter.eventPose->globalPose[1];

		// 	const PxVec3 body0LinearVelocityAtContactEvent = iter.preSolverVelocity->linearVelocity[0]
		// 		const PxVec3 body1LinearVelocityAtContactEvent = iter.preSolverVelocity->linearVelocity[1];

		// 	const PxVec3 body0AngularVelocityAtContactEvent = iter.preSolverVelocity->angularVelocity[0]
		// 		const PxVec3 body1AngularVelocityAtContactEvent = iter.preSolverVelocity->angularVelocity[1];
		// }
	}


}