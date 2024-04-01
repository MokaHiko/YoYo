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

	VillageProps& props = GetComponent<VillageProps>();
	props.max_villagers = 10;
	if (m_timer > props.spawn_rate)
	{
		if (m_villager_count < props.max_villagers)
		{
			SpawnVillager();
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

	if(!parent)
	{
		return;
	}

	// TODO: Pass as argument
	static yoyo::Mat4x4 skinned_mesh_transform = parent.GetComponent<TransformComponent>().model_matrix;
	const std::string joint_name = node->name;

	// Bind pose
	// Entity joint = Instantiate(joint_name.c_str(), {0.0f, 0.0f, 0.0f});
	// auto& mesh_renderer = joint.AddComponent<MeshRendererComponent>("cubeCube", "default_instanced_material");
	// parent.GetComponent<TransformComponent>().AddChild(joint);

	// yoyo::Mat4x4 transform_matrix = skinned_mesh_transform * joints[node->joint_index].bind_pose_transform;
	// joint.GetComponent<TransformComponent>().SetGlobalTransform(transform_matrix);
	// joint.GetComponent<TransformComponent>().scale = {1.0f, 1.0f, 1.0f};
	// const std::string joint_name = node->name;

	Entity joint = Instantiate(joint_name.c_str(), {node->transform});
	auto& mesh_renderer = joint.AddComponent<MeshRendererComponent>("cubeCube", "default_instanced_material");
	parent.GetComponent<TransformComponent>().AddChild(joint);
	joint.GetComponent<TransformComponent>().scale = {1.0f, 1.0f, 1.0f};

	// for (yoyo::SkeletalNode* child : node->children)
	// {
	// 	TraverseRecursive(child, joints, joint);
	// }
}

void VillageManagerComponent::SpawnVillager(const VillagerProps& props)
{
	//static auto villager_model = yoyo::ResourceManager::Instance().Load<yoyo::Model>("assets/models/Peasant.yo");
	//static auto skinned_villager_material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("skinned_colormap_material");
	//static auto villager_material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("people_material");
	//static auto grid_material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("grid_material");

	static auto skinned_debug_material = yoyo::ResourceManager::Instance().Load<yoyo::Material>("skinned_colormap_debug_material");
	static auto villager_model = yoyo::ResourceManager::Instance().Load<yoyo::Model>("assets/models/mutant.yo");
	auto villager = Instantiate("villager", { pos_generator.Next(), height_generator.Next(), pos_generator.Next()});
	TransformComponent& model_transform = villager.GetComponent<TransformComponent>();
	model_transform.scale *= 0.20f;

	for (int i = 0; i < villager_model->meshes.size(); i++)
	{
		yoyo::MeshType mesh_type = villager_model->meshes[i]->GetMeshType();

		if (mesh_type == yoyo::MeshType::Skinned)
		{
			Ref<yoyo::SkinnedMesh> mesh = std::static_pointer_cast<yoyo::SkinnedMesh>(villager_model->meshes[i]);

			Entity child = Instantiate(mesh->name, villager_model->model_matrices[i]);
			model_transform.AddChild(child);

			{
				auto& mesh_renderer = child.AddComponent<MeshRendererComponent>();
				mesh_renderer.mesh = mesh;
				mesh_renderer.type = mesh_type;
				mesh_renderer.material = skinned_debug_material; // skinned_villager_material;
			}

			const yoyo::SkeletalNode* node = mesh->skeletal_hierarchy->GetRoot();
			TraverseRecursive(node, mesh->joints, child);

			AnimatorComponent& animator = child.AddComponent<AnimatorComponent>();
			animator.animator->skinned_mesh = mesh;
			animator.animator->animations.push_back(yoyo::ResourceManager::Instance().Load<yoyo::Animation>("Armature|Armature|mixamo.com|Layer0"));
		}
	}

	// villager.AddComponent<psx::RigidBodyComponent>().LockRotationAxis({1,0,1});
	// psx::BoxColliderComponent& box_collider = villager.AddComponent<psx::BoxColliderComponent>();
	// villager.AddComponent<VillagerComponent>(villager);

	villager.AddComponent<DemoModelComponent>(villager);
	m_villager_count++;
}




