#include "PhysicsEvents.h"

#include <Core/Log.h>

namespace psx
{
	CollisionEvent::CollisionEvent(const Collision& col)
		:collision(col) {}
}