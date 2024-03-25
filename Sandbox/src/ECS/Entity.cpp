#include "Entity.h"

#include "Scene.h"
#include "Components/Components.h"

Entity::Entity(entt::entity id, Scene* scene)
	:m_id(id), m_scene(scene) {}

Entity::Entity(uint32_t id, Scene* scene)
	:m_id(static_cast<entt::entity>(id)), m_scene(scene) {}

const bool Entity::IsValid() const
{ 
	return m_scene != nullptr && m_id != entt::null && m_scene->Registry().valid(m_id);
}

std::ostream& operator<<(std::ostream& stream, Entity& e)
{
	TagComponent& tag = e.m_scene->GetComponent<TagComponent>(e.m_id);
	stream << e.Id() << ":" << tag.tag;

	return stream;
}