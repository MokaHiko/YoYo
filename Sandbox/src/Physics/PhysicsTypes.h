#pragma once

#include <Math/Math.h>

#include <PxPhysicsAPI.h>

namespace psx
{
	enum class RigidBodyType
	{
		Static,
		Kinematic,
		Dynamic,
	};

	enum class ForceMode
	{
		Force,				//!< parameter has unit of mass * length / time^2, i.e., a force
		Impulse,			//!< parameter has unit of mass * length / time, i.e., force * time
		VelocityChange,		//!< parameter has unit of length / time, i.e., the effect is mass independent: a velocity change.
		Acceleration		//!< parameter has unit of length/ time^2, i.e., an acceleration. it gets treated just like a force except the mass is not divided out before integration.
	};

	struct CollisionPoints
	{
		yoyo::Vec3 position;
		yoyo::Vec3 normal;
	};
	
	struct Collision
	{
		std::vector<CollisionPoints> collision_points;
		uint32_t a, b;
	};

	struct RaycastHit
	{
		yoyo::Vec3 normal = {};
		yoyo::Vec3 point = {};

		float distance;
		uint32_t entity_id;
	};

	class RigidBodyComponent
	{
	public:
		void AddForce(const yoyo::Vec3& force, ForceMode type);

		// Locks an axis of rotation
		void LockRotationAxis(const yoyo::Vec3& axis);

		// Returns the mass in kg
		const float GetMass() const;

		// Returns the mass in kg
		void SetMass(float mass);

		void SetAngularVelocity(const yoyo::Vec3& velocity);

		void SetLinearVelocity(const yoyo::Vec3& velocity);

		void SetMaxLinearVelocity(float max_velocity);

		void SetUseGravity(bool value);
	private:	
		friend class PhysicsWorld;
		physx::PxRigidActor* actor;
	};

	struct PhysicsMaterial
	{
		bool nice = false;
	};

	class BoxColliderComponent
	{
	public:
		BoxColliderComponent(const yoyo::Vec3& extents);

		BoxColliderComponent();
		~BoxColliderComponent() = default;

		void SetHalfExtents(const yoyo::Vec3& half_extents);
		const yoyo::Vec3& GetHalfExtents() const;
	private:	
		friend class BoxColliderSystem;
		yoyo::Vec3 m_extents;
		physx::PxBoxGeometry* m_box;
	};
}
