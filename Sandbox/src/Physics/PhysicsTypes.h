#pragma once

#include <PxPhysicsAPI.h>

namespace psx
{
	enum class RigidBodyType
	{

	};

	struct RigidBodyComponent
	{
		physx::PxRigidActor* actor;
	};
}
