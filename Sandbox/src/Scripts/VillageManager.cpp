#include "VillageManager.h"

#include <Math/Quaternion.h>
#include <Math/Random.h>
#include <Math/MatrixTransform.h>

#include <Resource/ResourceManager.h>

#include <Renderer/Model.h>
#include <Renderer/Material.h>
#include <Renderer/Texture.h>

#include "ECS/Components/RenderableComponents.h"

#include "Villager.h"
#include "Destructable.h"

static yoyo::PRNGenerator<float> angle_generator(0.0f, 360.0f);
static yoyo::PRNGenerator<float> pos_generator(-10.0f, 10.0f);
static yoyo::PRNGenerator<float> height_generator(2.0f, 15.0f);

VillageManagerComponent::VillageManagerComponent(Entity e)
	:ScriptableEntity(e)
{
}

VillageManagerComponent::~VillageManagerComponent() {}

void VillageManagerComponent::OnUpdate(float dt)
{
	m_timer += dt;

	const VillageProps& props = GetComponent<VillageProps>();
	if (m_timer > props.spawn_rate)
	{
		if (m_villager_count < props.max_villagers)
		{
			SpawnVillager();

			//VillageItem item;
			//SpawnItem(item);
		}

		m_timer = 0;
	}
}

void VillageManagerComponent::SpawnVillager(const VillagerProps& props)
{
	static auto villager_model = yoyo::ResourceManager::Instance().Load<yoyo::Model>("assets/models/villager.yo");
	static auto villager_material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("colormap_material");

	for (int i = 0; i < 1; i++)
	{
		auto villager = Instantiate("villager_" + std::to_string(i), { pos_generator.Next(), height_generator.Next(), pos_generator.Next() });
		TransformComponent& model_transform = villager.GetComponent<TransformComponent>();
		model_transform.scale *= 0.1f;
		model_transform.quat_rotation = yoyo::QuatFromAxisAngle({0, 1, 0}, Y_PI);

		for (int j = 0; j < villager_model->meshes.size(); j++)
		{
			Entity mesh = Instantiate(villager_model->meshes[j]->name);
			TransformComponent& transform = mesh.GetComponent<TransformComponent>();
			transform.position = yoyo::PositionFromMat4x4(villager_model->model_matrices[j]);
			transform.scale = yoyo::ScaleFromMat4x4(villager_model->model_matrices[j]);

			auto& mesh_renderer = mesh.AddComponent<MeshRendererComponent>();
			mesh_renderer.mesh = villager_model->meshes[j];
			mesh_renderer.material = villager_material;

			model_transform.AddChild(mesh);
		}

		villager.AddComponent<psx::RigidBodyComponent>().LockRotationAxis({1,0,1});
		psx::BoxColliderComponent& box_collider = villager.AddComponent<psx::BoxColliderComponent>();

		villager.AddComponent<DestructableComponent>(villager);
		villager.AddComponent<VillagerComponent>(villager);

		m_villager_count++;
	}
}

void VillageManagerComponent::SpawnItem(const VillageItem& item)
{
	static auto colormap_material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("colormap_material");
	static auto pine_model = yoyo::ResourceManager::Instance().Load<yoyo::Model>("assets/models/pine.yo");

	for (int i = 0; i < 1; i++)
	{
		auto pine = Instantiate("pine_" + std::to_string(i), { pos_generator.Next(), height_generator.Next(), pos_generator.Next() });
		TransformComponent& model_transform = pine.GetComponent<TransformComponent>();
		model_transform.scale *= 5.0f;

		for (int j = 0; j < pine_model->meshes.size(); j++)
		{
			Entity mesh = Instantiate(pine_model->meshes[j]->name);
			TransformComponent& transform = mesh.GetComponent<TransformComponent>();
			transform.position = yoyo::PositionFromMat4x4(pine_model->model_matrices[j]);
			transform.scale = yoyo::ScaleFromMat4x4(pine_model->model_matrices[j]);

			auto& mesh_renderer = mesh.AddComponent<MeshRendererComponent>();
			mesh_renderer.mesh = pine_model->meshes[j];
			mesh_renderer.material = colormap_material;

			model_transform.AddChild(mesh);
		}
		pine.AddComponent<psx::RigidBodyComponent>();
		psx::BoxColliderComponent& box_collider = pine.AddComponent<psx::BoxColliderComponent>();

		pine.AddComponent<DestructableComponent>(pine);
	}
}