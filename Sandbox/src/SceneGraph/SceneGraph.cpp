#include "SceneGraph.h"

#include "SceneGraph.h"

#include "Core/Application.h"
#include "ECS/Entity.h "

SceneGraph::SceneGraph(Scene* scene)
	:m_scene(scene)
{
}

SceneGraph::~SceneGraph() {}

void SceneGraph::Init() 
{
	m_scene->Registry().on_construct<TransformComponent>().connect<&SceneGraph::OnTransformCreated>(this);
	m_scene->Registry().on_destroy<TransformComponent>().connect<&SceneGraph::OnTransformDestroyed>(this);
}

void SceneGraph::Shutdown() 
{
	// Unsubscribe
}

void SceneGraph::Update(TransformComponent& root, float dt)
{
	RecursiveUpdate(root);
}

void SceneGraph::OnTransformCreated(entt::basic_registry<entt::entity>&, entt::entity entity)
{
	Entity e(entity, m_scene);

	// Assign self to transform
	TransformComponent& transform = e.GetComponent<TransformComponent>();
	transform.self = e;
}

void SceneGraph::OnTransformDestroyed(entt::basic_registry<entt::entity>&, entt::entity entity)
{
	Entity e(entity, m_scene);

	TransformComponent& transform = e.GetComponent<TransformComponent>();

	if(transform.parent)
	{
		transform.parent.GetComponent<TransformComponent>().RemoveChild(e);
	}

	// Destroy children
	for (uint32_t i = 0; i < transform.children_count; i++)
	{
		m_scene->QueueDestroy(transform.children[i]);
	}
}

void SceneGraph::RecursiveUpdate(TransformComponent& node)
{
	for(uint32_t i = 0; i < node.children_count; i++)
	{
		TransformComponent& transform = node.children[i].GetComponent<TransformComponent>();

		// TODO: Dirty Check
		transform.UpdateModelMatrix();

		RecursiveUpdate(transform);
	}
}

void SceneGraph::OnModelRendererCreated(entt::basic_registry<entt::entity> &, entt::entity entity) 
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
