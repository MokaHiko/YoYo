#include "PhysicsTypes.h"

namespace psx
{
	void RigidBodyComponent::AddForce(const yoyo::Vec3& force, ForceMode type)
	{
		using namespace physx;

		// TODO: Queue add force of out of simulation
		actor->is<PxRigidBody>()->addForce({ force.x, force.y, force.z }, PxForceMode::eIMPULSE);
	}

	void RigidBodyComponent::LockRotationAxis(const yoyo::Vec3& axis)
	{
		using namespace physx;

		yoyo::Vec3 lock_axis = yoyo::Normalize(axis);
		PxRigidDynamicLockFlags flags = {};
		flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;

		if(lock_axis.x)
		{
			flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
		}

		if(lock_axis.y)
		{
			flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
		}

		if(lock_axis.z)
		{
			flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
		}

		actor->is<PxRigidDynamic>()->setRigidDynamicLockFlags(flags);
	}

	const float RigidBodyComponent::GetMass() const
	{
		using namespace physx;

		if (!actor)
		{
			return -1.0f;
		}

		if (float mass = actor->is<PxRigidDynamic>()->getMass())
		{
			return mass;
		}
		else
		{
			return -1.0f;
		}
	}

	void RigidBodyComponent::SetMass(float mass)
	{
		using namespace physx;

		if(!actor)
		{
			return;
		}

		if (PxRigidDynamic* rb_dynamic = actor->is<PxRigidDynamic>())
		{
			rb_dynamic->setMass(mass);
		}
	}

	BoxColliderComponent::BoxColliderComponent()
		:m_box(nullptr), m_extents(yoyo::Vec3{ 1.0f, 1.0f, 1.0f }) {}

	void BoxColliderComponent::SetHalfExtents(const yoyo::Vec3& half_extents)
	{
		if (m_box)
		{
			m_box->halfExtents.x = half_extents.x;
			m_box->halfExtents.y = half_extents.y;
			m_box->halfExtents.z = half_extents.z;
		}

		m_extents = half_extents;
	}

	const yoyo::Vec3& BoxColliderComponent::GetHalfExtents() const
	{
		return m_extents;
	}
}