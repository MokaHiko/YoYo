#include "Entity.h"

#include "Scene.h"
#include "Components/Components.h"

namespace yoyo
{
	Entity::Entity(entt::entity id, Scene* scene)
		:m_id(id), m_scene(scene)
	{
	}

	std::ostream& operator<<(std::ostream& stream, Entity& e)
	{
		Tag& tag = e.m_scene->GetComponent<Tag>(e.m_id);
		stream << e.Id() << ":" << tag.name;

		return stream;
	}
}