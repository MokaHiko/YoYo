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
#include "DemoModel.h"

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

			VillageItem item;
			SpawnItem(item);
		}

		m_timer = 0;
	}
}

void VillageManagerComponent::SpawnVillager(const VillagerProps& props)
{
	static auto villager_model = yoyo::ResourceManager::Instance().Load<yoyo::Model>("assets/models/Peasant.yo");
	static auto villager_material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("colormap_material");

	//static auto skinned_villager_material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("skinned_colormap_material");
	static auto skinned_villager_debug_material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("skinned_colormap_debug_material");
	static auto grid_material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("grid_material");

	for (int i = 0; i < 1; i++)
	{
		auto villager = Instantiate("villager_" + std::to_string(i), { pos_generator.Next(), height_generator.Next(), pos_generator.Next() });
		TransformComponent& model_transform = villager.GetComponent<TransformComponent>();

		for (int j = 0; j < villager_model->meshes.size(); j++)
		{
			yoyo::MeshType mesh_type = villager_model->meshes[j]->GetMeshType();

			if(mesh_type == yoyo::MeshType::Static)
			{
				Ref<yoyo::StaticMesh> mesh = std::static_pointer_cast<yoyo::StaticMesh>(villager_model->meshes[j]);
				Entity child = Instantiate(mesh->name);

				TransformComponent& transform = child.GetComponent<TransformComponent>();
				transform.position = yoyo::PositionFromMat4x4(villager_model->model_matrices[j]);
				transform.scale = yoyo::ScaleFromMat4x4(villager_model->model_matrices[j]);

				auto& mesh_renderer = child.AddComponent<MeshRendererComponent>();
				mesh_renderer.mesh = mesh;
				mesh_renderer.type = mesh_type;
				mesh_renderer.material = villager_material;

				model_transform.AddChild(child);
			}
			else if(mesh_type == yoyo::MeshType::Skinned)
			{
				Ref<yoyo::SkinnedMesh> mesh = std::static_pointer_cast<yoyo::SkinnedMesh>(villager_model->meshes[j]);
				Entity child = Instantiate(mesh->name);

				TransformComponent& transform = child.GetComponent<TransformComponent>();
				transform.position = yoyo::PositionFromMat4x4(villager_model->model_matrices[j]);
				transform.scale = yoyo::ScaleFromMat4x4(villager_model->model_matrices[j]);

				{
					auto& mesh_renderer = child.AddComponent<MeshRendererComponent>();
					mesh_renderer.mesh = mesh;
					mesh_renderer.type = mesh_type;
					//mesh_renderer.material = skinned_villager_material;
					mesh_renderer.material = skinned_villager_debug_material;
				}

				for(auto& bMatrix : mesh->bones)
				{	
					const std::string bone_name = "bone";
					Entity bone = Instantiate(bone_name.c_str());
					TransformComponent& bone_transform = bone.GetComponent<TransformComponent>();
					bone_transform.position = yoyo::PositionFromMat4x4(bMatrix);
					bone_transform.scale *= 0.05f;

					auto& mesh_renderer = bone.AddComponent<MeshRendererComponent>("cubeCube", "default_instanced_material");;
					model_transform.AddChild(bone);
				}

				model_transform.AddChild(child);
			}
		}

		// villager.AddComponent<psx::RigidBodyComponent>().LockRotationAxis({1,0,1});
		// psx::BoxColliderComponent& box_collider = villager.AddComponent<psx::BoxColliderComponent>();
		// villager.AddComponent<VillagerComponent>(villager);

		model_transform.position = {0.0f, 0.0f, 0.0f};
		model_transform.scale *= 15.0f;
		villager.AddComponent<DemoModelComponent>(villager);

		m_villager_count++;
	}
}

void VillageManagerComponent::SpawnItem(const VillageItem& item)
{
	//static auto colormap_material = yoyo::resourcemanager::instance().load<yoyo::material>("colormap_material");
	//static auto pine_model = yoyo::resourcemanager::instance().load<yoyo::model>("assets/models/pine.yo");

	//for (int i = 0; i < 1; i++)
	//{
	//	auto pine = Instantiate("pine_" + std::to_string(i), { pos_generator.Next(), height_generator.Next(), pos_generator.Next() });
	//	TransformComponent& model_transform = pine.GetComponent<TransformComponent>();
	//	model_transform.scale *= 5.0f;

	//	for (int j = 0; j < pine_model->meshes.size(); j++)
	//	{
	//		Entity mesh = Instantiate(pine_model->meshes[j]->name);
	//		TransformComponent& transform = mesh.GetComponent<TransformComponent>();
	//		transform.position = yoyo::PositionFromMat4x4(pine_model->model_matrices[j]);
	//		transform.scale = yoyo::ScaleFromMat4x4(pine_model->model_matrices[j]);

	//		auto& mesh_renderer = mesh.AddComponent<MeshRendererComponent>();
	//		mesh_renderer.mesh = pine_model->meshes[j];
	//		mesh_renderer.material = colormap_material;

	//		model_transform.AddChild(mesh);
	//	}
	//	pine.AddComponent<psx::RigidBodyComponent>();
	//	psx::BoxColliderComponent& box_collider = pine.AddComponent<psx::BoxColliderComponent>();

	//	pine.AddComponent<DestructableComponent>(pine);
	//}
}