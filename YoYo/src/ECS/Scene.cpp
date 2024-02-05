#include "Scene.h"

#include "Components/Components.h"

namespace yoyo
{
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::Instantiate(const std::string& name, const Vec3& position, bool* serialize)
	{
		Entity e = {};
		e = Entity{ m_registry.create(), this };

		auto& tag = e.AddComponent<Tag>();
		tag.name = name;

		auto& transform = e.AddComponent<Transform>();
		transform.position = position;

		return e;
	}

	entt::registry& Scene::Registry()
	{
		return m_registry;
	}
}
