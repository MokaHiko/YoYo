#include "Collider.h"
#include "Core/Assert.h"

#include "Physics3D.h"

namespace psx
{
	BoxColliderSystem::BoxColliderSystem(Scene* scene, PhysicsWorld& world)
		:System<BoxColliderComponent>(scene), m_world(world)
	{
	}

	BoxColliderSystem::~BoxColliderSystem()
	{
	}

	void BoxColliderSystem::OnComponentCreated(Entity e, BoxColliderComponent& box_collider)
	{
		using namespace physx;

		if(!e.HasComponent<RigidBodyComponent>())
		{
			YERROR("Cannot add box collider component to an entity with no rigidbody!");
			return;
		}

		RigidBodyComponent& rb = e.GetComponent<RigidBodyComponent>();
		const yoyo::Vec3 extents = box_collider.GetHalfExtents();

		m_world.AttachBoxShape(rb, extents, &box_collider.m_box);
	}

	void BoxColliderSystem::OnComponentDestroyed(Entity e, BoxColliderComponent& box_collider)
	{
		using namespace physx;

		YASSERT(e.HasComponent<RigidBodyComponent>(), "Entity has no rigidbody componet");

		if(!e.HasComponent<BoxColliderComponent>())
		{
			YERROR("Cannot remove collider component to an entity with no rigidbody!");
		}

		RigidBodyComponent& rb = e.GetComponent<RigidBodyComponent>();
		//m_world.DetatchShape(rb, box_colldier);
	}
}
