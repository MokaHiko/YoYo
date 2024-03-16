#include "Scene.h"

#include "Core/Assert.h"
#include "Components/Components.h"

Scene::Scene()
{
	m_root = Entity{ m_registry.create(), this};

	auto& tag = m_root.AddComponent<TagComponent>();
	tag.tag = "root";

	TransformComponent& transform = m_root.AddComponent<TransformComponent>();
	transform.position = {0.0f, 0.0f, 0.0f};
	transform.UpdateModelMatrix();
	transform.self = m_root;
}

Scene::~Scene()
{

}

Entity Scene::Root()
{
	YASSERT(m_root, "Scene has invalid root!");

	return m_root;
}

Entity Scene::Instantiate(const std::string& name, const yoyo::Vec3& position, bool* serialize)
{
	Entity e = {};
	e = Entity{ m_registry.create(), this };

	auto& tag = e.AddComponent<TagComponent>();
	tag.tag = name;

	auto& transform = e.AddComponent<TransformComponent>();
	transform.position = position;

	// Add to root of scene
	auto& root_transform = Root().GetComponent<TransformComponent>();
	root_transform.AddChild(e);

	return e;
}

void Scene::QueueDestroy(Entity e) 
{
	m_destruction_queue.push_back(e);
}

entt::registry &Scene::Registry() { return m_registry; }
