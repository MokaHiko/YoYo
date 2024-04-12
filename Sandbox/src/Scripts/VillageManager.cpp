#include "VillageManager.h"

#include <Math/Quaternion.h>
#include <Math/Random.h>
#include <Math/MatrixTransform.h>

#include <Resource/ResourceManager.h>

#include <Renderer/Model.h>
#include <Renderer/Material.h>
#include <Renderer/Texture.h>
#include <Renderer/Animation.h>

#include "ECS/Components/RenderableComponents.h"

#include "Villager.h"
#include "Destructable.h"

#include "Unit.h"
#include "UnitController.h"
#include "Enemy.h"

static yoyo::PRNGenerator<float> angle_generator(0.0f, 360.0f);
static yoyo::PRNGenerator<float> pos_generator(-100.0f, 100.0f);
static yoyo::PRNGenerator<float> height_generator(2.0f, 15.0f);

VillageManagerComponent::VillageManagerComponent(Entity e)
	:ScriptableEntity(e)
{
}

VillageManagerComponent::~VillageManagerComponent() {}

void VillageManagerComponent::OnUpdate(float dt)
{
	m_timer += dt;

	VillageProps& props = GetComponent<VillageProps>();
	if (m_timer > props.spawn_rate) 
	{
		if (m_villager_count < props.max_villagers)
		{
			SpawnVillager();

			for(int i = 0; i < 0; i++)
			{
				SpawnEnemy();
			}
		}

		m_timer = 0;
	}
}

void VillageManagerComponent::TraverseRecursive(const yoyo::SkeletalNode* node, const std::vector<yoyo::SkinnedMeshJoint>& joints, Entity parent)
{
	if (!node)
	{
		return;
	}

	if (!parent)
	{
		return;
	}

	const std::string joint_name = node->name;
	Entity joint = Instantiate(joint_name.c_str(), {node->transform});
	parent.GetComponent<TransformComponent>().AddChild(joint);

	//auto& mesh_renderer = joint.AddComponent<MeshRendererComponent>("Cube", "default_material");

	for (yoyo::SkeletalNode* child : node->children)
	{
		TraverseRecursive(child, joints, joint);
	}
}

void VillageManagerComponent::SpawnVillager(const VillagerProps& props)
{
	static auto villager_model = yoyo::ResourceManager::Instance().Load<yoyo::Model>("assets/models/Humanoid.yo");
	static auto skinned_villager_material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("skinned_people_material");

	auto villager = Instantiate("villager", { 0.0f, 0.0f, 0.0f });
	TransformComponent& model_transform = villager.GetComponent<TransformComponent>();
	model_transform.scale *= 0.05f;

	for (int i = 0; i < villager_model->meshes.size(); i++)
	{
		yoyo::MeshType mesh_type = villager_model->meshes[i]->GetMeshType();

		if (mesh_type == yoyo::MeshType::Skinned)
		{
			Ref<yoyo::SkinnedMesh> mesh = std::static_pointer_cast<yoyo::SkinnedMesh>(villager_model->meshes[i]);

			Entity child = Instantiate(mesh->name, villager_model->model_matrices[i]);
			model_transform.AddChild(child);

			auto& mesh_renderer = child.AddComponent<MeshRendererComponent>();
			mesh_renderer.mesh = mesh;
			mesh_renderer.type = mesh_type;
			mesh_renderer.material = skinned_villager_material;

			// TODO: Fix animator component updating multiple times each component
			AnimatorComponent& animator = child.AddComponent<AnimatorComponent>();
			animator.animator->skinned_mesh = mesh;
			animator.animator->animations.push_back(yoyo::ResourceManager::Instance().Load<yoyo::Animation>("assets/animations/VillagerRunning.yanimation"));
			animator.animator->animations.push_back(yoyo::ResourceManager::Instance().Load<yoyo::Animation>("assets/animations/HipHopDancing.yanimation"));
		}
	}

	villager.AddComponent<psx::RigidBodyComponent>().LockRotationAxis({1,0,1});
	psx::BoxColliderComponent& box_collider = villager.AddComponent<psx::BoxColliderComponent>();

	villager.AddComponent<Unit>(villager);
	villager.AddComponent<UnitController>(villager);
	villager.AddComponent<VillagerComponent>(villager);

	m_villager_count++;
}

void VillageManagerComponent::SpawnEnemy() 
{
	static auto villager_model = yoyo::ResourceManager::Instance().Load<yoyo::Model>("assets/models/Humanoid.yo");
	static auto skinned_villager_material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("skinned_people_material");

	//auto villager = Instantiate("enemy_villager", { 10.0f, 0.0f, 0.0f });
	auto villager = Instantiate("enemy_villager", { pos_generator.Next(), 0.0f, pos_generator.Next() });
	TransformComponent& model_transform = villager.GetComponent<TransformComponent>();
	model_transform.scale *= 0.05f;

	for (int i = 0; i < villager_model->meshes.size(); i++)
	{
		yoyo::MeshType mesh_type = villager_model->meshes[i]->GetMeshType();

		if (mesh_type == yoyo::MeshType::Skinned)
		{
			Ref<yoyo::SkinnedMesh> mesh = std::static_pointer_cast<yoyo::SkinnedMesh>(villager_model->meshes[i]);

			Entity child = Instantiate(mesh->name, villager_model->model_matrices[i]);
			model_transform.AddChild(child);

			auto& mesh_renderer = child.AddComponent<MeshRendererComponent>();
			mesh_renderer.mesh = mesh;
			mesh_renderer.type = mesh_type;
			mesh_renderer.material = skinned_villager_material;

			// TODO: Fix animator component updating multiple times each component
			// AnimatorComponent& animator = child.AddComponent<AnimatorComponent>();
			// animator.animator->skinned_mesh = mesh;
			// animator.animator->animations.push_back(yoyo::ResourceManager::Instance().Load<yoyo::Animation>("assets/animations/VillagerRunning.yanimation"));
			// animator.animator->animations.push_back(yoyo::ResourceManager::Instance().Load<yoyo::Animation>("assets/animations/HipHopDancing.yanimation"));
		}
	}

	villager.AddComponent<psx::RigidBodyComponent>().LockRotationAxis({1,0,1});
	psx::BoxColliderComponent& box_collider = villager.AddComponent<psx::BoxColliderComponent>();

	villager.AddComponent<Unit>(villager).GetMovementStats().ms *= 0.80f;
	villager.AddComponent<UnitController>(villager);
	villager.AddComponent<Enemy>(villager);
}

void VillageManagerComponent::SpawnMutant()
{
	static auto skinned_debug_material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("skinned_colormap_debug_material");
	static auto skinned_mutant_material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("skinned_mutant_material");

	static auto mutant_model = yoyo::ResourceManager::Instance().Load<yoyo::Model>("assets/models/mutant.yo");
	auto mutant = Instantiate("mutant", { pos_generator.Next(), height_generator.Next(), pos_generator.Next()});

	TransformComponent& model_transform = mutant.GetComponent<TransformComponent>();
	model_transform.scale *= 0.1f;

	for (int i = 0; i < mutant_model->meshes.size(); i++)
	{
		yoyo::MeshType mesh_type = mutant_model->meshes[i]->GetMeshType();

		if (mesh_type == yoyo::MeshType::Skinned)
		{
			Ref<yoyo::SkinnedMesh> mesh = std::static_pointer_cast<yoyo::SkinnedMesh>(mutant_model->meshes[i]);

			Entity child = Instantiate(mesh->name, mutant_model->model_matrices[i]);
			model_transform.AddChild(child);

			{
				auto& mesh_renderer = child.AddComponent<MeshRendererComponent>();
				mesh_renderer.mesh = mesh;
				mesh_renderer.type = mesh_type;
				mesh_renderer.material = skinned_mutant_material;
			}

			static bool do_once = [&]()
			{
				AnimatorComponent& animator = child.AddComponent<AnimatorComponent>();
				animator.animator->skinned_mesh = mesh;
				animator.animator->animations.push_back(yoyo::ResourceManager::Instance().Load<yoyo::Animation>("slash"));
				return true;
			}();
		}
	}

	psx::RigidBodyComponent& rb = mutant.AddComponent<psx::RigidBodyComponent>();
	rb.LockRotationAxis({1,1,1});
	psx::BoxColliderComponent& box_collider = mutant.AddComponent<psx::BoxColliderComponent>(yoyo::Vec3{5.0f, 1.0f, 5.0f});

	mutant.AddComponent<Unit>(mutant);
	mutant.AddComponent<UnitController>(mutant);

	m_villager_count++;
}
