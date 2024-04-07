#include "Physics3D.h"

#include <Core/Log.h>
#include <Core/Time.h>
#include <Events/Event.h>

#include "ECS/Components/Components.h"
#include "PhysicsEvents.h"
#include "Collider.h"

namespace psx
{
	using namespace physx;

	static PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{
		PX_UNUSED(attributes0);
		PX_UNUSED(attributes1);
		PX_UNUSED(filterData0);
		PX_UNUSED(filterData1);
		PX_UNUSED(constantBlockSize);
		PX_UNUSED(constantBlock);

		// all initial and persisting reports for everything, with per-point data
		pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT
			| PxPairFlag::eNOTIFY_TOUCH_FOUND
			| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
			| PxPairFlag::eNOTIFY_CONTACT_POINTS;
		return PxFilterFlag::eDEFAULT;
	}

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

	void PhysicsWorld::AttachBoxShape(RigidBodyComponent& rb, const yoyo::Vec3& extents, physx::PxBoxGeometry** box_shape, PhysicsMaterial* material)
	{
		using namespace physx;

		// TODO: Cache Shape: 
		PxShape* shape = m_physics->createShape(PxBoxGeometry({ extents.x, extents.y, extents.z }), *m_material);
		*box_shape = (physx::PxBoxGeometry*)shape;

		rb.actor->attachShape(*shape);

		shape->release();
	}

	bool PhysicsWorld::Raycast(const yoyo::Vec3& origin, const yoyo::Vec3& dir, float max_distance, RaycastHit& out)
	{
		using namespace physx;

		PxRaycastBuffer hit;

		if (!m_scene->raycast({ origin.x, origin.y, origin.z }, { dir.x, dir.y, dir.z }, max_distance, hit))
		{
			return false;
		}

		out.distance = hit.block.distance;
		out.normal = { hit.block.normal.x, hit.block.normal.y, hit.block.normal.z };
		out.point = { hit.block.position.x, hit.block.position.y, hit.block.position.z };
		out.entity_id = (uint32_t)hit.block.actor->userData;

		return true;
	}
	
	PhysicsWorld::PhysicsWorld(Scene* scene)
		:System<RigidBodyComponent>(scene)
	{
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
		scene_desc.filterShader = contactReportFilterShader;
		m_scene = m_physics->createScene(scene_desc);

		m_simulation_event_callback = YNEW SimulationEventCallback();
		m_scene->setSimulationEventCallback(m_simulation_event_callback);

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

		// Init subsystems
		m_box_collider_system = CreateRef<BoxColliderSystem>(GetScene(), *this);
		m_box_collider_system->Init();

		// for (PxU32 i = 0;i < 5;i++)
		// {
		// 	CreateStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);
		// }

		// if (!false)
		// 	CreateDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));
	}

	void PhysicsWorld::Shutdown()
	{
		m_box_collider_system->Shutdown();

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

			if(!m_scene->simulate(1.0f / 60.0f))
			{
				YERROR("Physics step failed!");
			}

			m_scene->fetchResults(true);
		}

		{
			using namespace physx;
			for (auto entity : GetScene()->Registry().group<TransformComponent, RigidBodyComponent>())
			{
				Entity e(entity, GetScene());
				TransformComponent& transform = e.GetComponent<TransformComponent>();
				const RigidBodyComponent& rb = e.GetComponent<RigidBodyComponent>();

				PxTransform t = rb.actor->getGlobalPose();
				transform.position = { t.p.x, t.p.y, t.p.z };
				transform.quat_rotation = { t.q.x, t.q.y, t.q.z, t.q.w };
			}
		}
	}

	void PhysicsWorld::OnComponentCreated(Entity e, RigidBodyComponent& rb)
	{
		using namespace physx;

		const TransformComponent& transform = e.GetComponent<TransformComponent>();

		PxQuat q = { transform.quat_rotation.x, transform.quat_rotation.y, transform.quat_rotation.z, transform.quat_rotation.w };
		PxTransform t = { transform.position.x, transform.position.y, transform.position.z, q };

		rb.actor = m_physics->createRigidDynamic(t);
		rb.actor->userData = (void*)(uint64_t)(e.Id());
		m_scene->addActor(*rb.actor);

		PxRigidBodyExt::updateMassAndInertia(*(rb.actor->is<PxRigidBody>()), 10.0f);
	}

	void PhysicsWorld::OnComponentDestroyed(Entity e, RigidBodyComponent& rb)
	{
		using namespace physx;
		const TransformComponent& transform = e.GetComponent<TransformComponent>();
		PxTransform t = { transform.position.x, transform.position.y, transform.position.z };

		// Implicit release of shapes
		rb.actor->release();
	}

	void SimulationEventCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
	{
		YINFO("physics trigger event!");
	}

	void SimulationEventCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
	{
		//PX_UNUSED((pairHeader));
		const PxTransform body0PoseAtEndOfSimulateStep = pairHeader.actors[0]->is<PxRigidActor>()->getGlobalPose();
		const PxTransform body1PoseAtEndOfSimulateStep = pairHeader.actors[1]->is<PxRigidActor>()->getGlobalPose();

		uint32_t e1 = (uint32_t)pairHeader.actors[0]->userData;
		uint32_t e2 = (uint32_t)pairHeader.actors[1]->userData;

		Collision col{};
		col.a = e1;
		col.b = e2;

		static std::vector<PxContactPairPoint> contactPoints;
		for (PxU32 i = 0;i < nbPairs;i++)
		{
			PxU32 contactCount = pairs[i].contactCount;
			if (contactCount)
			{
				contactPoints.resize(contactCount);
				pairs[i].extractContacts(&contactPoints[0], contactCount);

				for (PxU32 j = 0;j < contactCount;j++)
				{
					//gContactPositions.push_back(contactPoints[j].position);
					//gContactImpulses.push_back(contactPoints[j].impulse);
					col.collision_points.push_back({ {contactPoints[j].position.x, contactPoints[j].position.y, contactPoints[j].position.z}, {contactPoints[j].normal.x, contactPoints[j].normal.y, contactPoints[j].normal.z} });
				}
			}
		}

		// TODO: Cache common physics events
		static Ref<CollisionEvent> col_event = CreateRef<CollisionEvent>(col);
		col_event->collision = col;
		yoyo::EventManager::Instance().Dispatch(col_event);

		//// Retrieve the current poses and velocities of the two actors involved in the contact event.
		//{
		//	const PxTransform body0PoseAtEndOfSimulateStep = pairHeader.actors[0]->is<PxRigidActor>()->getGlobalPose();
		//	const PxTransform body1PoseAtEndOfSimulateStep = pairHeader.actors[1]->is<PxRigidActor>()->getGlobalPose();

		//	const PxVec3 body0LinVelAtEndOfSimulateStep = pairHeader.actors[0]->is<PxRigidDynamic>() ? pairHeader.actors[0]->is<PxRigidDynamic>()->getLinearVelocity() : PxVec3(PxZero);
		//	const PxVec3 body1LinVelAtEndOfSimulateStep = pairHeader.actors[0]->is<PxRigidDynamic>() ? pairHeader.actors[1]->is<PxRigidDynamic>()->getLinearVelocity() : PxVec3(PxZero);

		//	const PxVec3 body0AngVelAtEndOfSimulateStep = pairHeader.actors[0]->is<PxRigidDynamic>() ? pairHeader.actors[0]->is<PxRigidDynamic>()->getAngularVelocity() : PxVec3(PxZero);
		//	const PxVec3 body1AngVelAtEndOfSimulateStep = pairHeader.actors[0]->is<PxRigidDynamic>() ? pairHeader.actors[1]->is<PxRigidDynamic>()->getAngularVelocity() : PxVec3(PxZero);

		//	auto e1 = (uint32_t)(pairHeader.actors[0]->userData);
		//	auto e2 = (uint32_t)(pairHeader.actors[1]->userData);
		//	YINFO("ASSSSS!");
		//	YINFO("%d - %d collision", e1, e2);
		//}

		//// Retrieve the poses and velocities of the two actors involved in the contact event as they were
		//// when the contact event was detected.

		//PxContactPairExtraDataIterator iter(pairHeader.extraDataStream, pairHeader.extraDataStreamSize);
		//while (iter.nextItemSet())
		//{
		//	const PxTransform body0PoseAtContactEvent = iter.eventPose->globalPose[0];
		//	const PxTransform body1PoseAtContactEvent = iter.eventPose->globalPose[1];

		//	const PxVec3 body0LinearVelocityAtContactEvent = iter.preSolverVelocity->linearVelocity[0];
		//	const PxVec3 body1LinearVelocityAtContactEvent = iter.preSolverVelocity->linearVelocity[1];

		//	const PxVec3 body0AngularVelocityAtContactEvent = iter.preSolverVelocity->angularVelocity[0];
		//	const PxVec3 body1AngularVelocityAtContactEvent = iter.preSolverVelocity->angularVelocity[1];
		//}
	}
}