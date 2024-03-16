#include "SceneGraph.h"

#include <imgui.h>
#include "SceneGraph.h"

#include "Core/Application.h"
#include "ECS/Entity.h "

void SceneGraph::Init()
{
}

void SceneGraph::Shutdown()
{
	// Unsubscribe
}

void SceneGraph::Update(float dt)
{
	TransformComponent& root = GetScene()->Root().GetComponent<TransformComponent>();
	RecursiveUpdate(root);
}

void SceneGraph::OnComponentCreated(Entity e, TransformComponent& transform)
{
	// Define self
	transform.self = e;
}

void SceneGraph::OnComponentDestroyed(Entity e, TransformComponent& transform)
{
	if (transform.parent)
	{
		transform.parent.GetComponent<TransformComponent>().RemoveChild(e);
	}

	// Destroy children
	for (uint32_t i = 0; i < transform.children_count; i++)
	{
		GetScene()->QueueDestroy(transform.children[i]);
	}
}

void SceneGraph::RecursiveUpdate(TransformComponent& node)
{
	const std::string name = node.self.GetComponent<TagComponent>().tag;
	for (uint32_t i = 0; i < node.children_count; i++)
	{
		TransformComponent& transform = node.children[i].GetComponent<TransformComponent>();

		// TODO: Dirty Check
		transform.UpdateModelMatrix();

		RecursiveUpdate(transform);
	}
}

void OnModelRendererCreated(entt::basic_registry<entt::entity>&, entt::entity entity)
{
	// Entity model_entity(entity, m_scene);
	// Transform& model_transform = model_entity.GetComponent<Transform>();

	// Ref<Model> model = model_entity.GetComponent<ModelRenderer>().model;

	// for (auto& model_mesh : model->_meshes)
	// {
	// 	bool serialize = false;
	// 	Entity e = m_scene->Instantiate(model_mesh.mesh->_mesh_name, {}, &serialize);

	// 	auto& mesh_renderer = e.AddComponent<MeshRenderer>(model_mesh.mesh, model->_materials[model_mesh.material_index]);
	// 	model_transform.AddChild(e);
	// }
}
