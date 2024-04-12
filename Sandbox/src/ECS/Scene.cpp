#include "Scene.h"

#include "Core/Assert.h"
#include "Components/Components.h"
#include "Math/MatrixTransform.h"

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

Entity Scene::Instantiate(const std::string & name, const yoyo::Mat4x4 & transform_matrix, bool* serialize)
{
	Entity e = {};
	e = Entity{ m_registry.create(), this };

	auto& tag = e.AddComponent<TagComponent>();
	tag.tag = name;

	auto& transform = e.AddComponent<TransformComponent>();
	transform.position = yoyo::PositionFromMat4x4(transform_matrix);
	transform.scale = yoyo::ScaleFromMat4x4(transform_matrix);
	transform.quat_rotation = yoyo::RotationFromMat4x4(transform_matrix);

	// Add to root of scene
	auto& root_transform = Root().GetComponent<TransformComponent>();
	root_transform.AddChild(e);
	return e;
}

void Scene::QueueDestroy(Entity e) 
{
	// TODO: Check if current entity already queued for destruction
	m_destruction_queue.push_back(e);

	// use for scripting
	// for(auto e : *m_registry.storage(0))
	// {

	// }
}

void Scene::FlushDestructionQueue() 
{
	for(auto e : m_destruction_queue)
	{
		Destroy(e);
	}

	m_destruction_queue.clear();
}

void Scene::Destroy(Entity e) 
{
	m_registry.destroy(e);
}

entt::registry &Scene::Registry() { return m_registry; }
