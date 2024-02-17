#include "Scene.h"

#include "Components/Components.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

Entity Scene::Instantiate(const std::string& name, const yoyo::Vec3& position, bool* serialize)
{
	Entity e = {};
	e = Entity{ m_registry.create(), this };

	auto& tag = e.AddComponent<TagComponent>();
	tag.tag = name;

	auto& transform = e.AddComponent<TransformComponent>();
	transform.position = position;

	return e;
}

entt::registry& Scene::Registry()
{
	return m_registry;
}
