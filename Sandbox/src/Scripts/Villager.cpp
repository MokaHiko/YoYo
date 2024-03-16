#include "Villager.h"

#include <Math/Quaternion.h>
#include <Input/Input.h>

#include "CameraController.h"

#include "ECS/Components/RenderableComponents.h"

VillagerComponent::VillagerComponent(Entity e)
	:ScriptableEntity(e) {}

VillagerComponent::~VillagerComponent() {}

void VillagerComponent::OnCreate() {}

void VillagerComponent::OnStart() 
{
	FindEntityWithComponent<CameraControllerComponent>().GetComponent<CameraControllerComponent>().follow = GameObject();
}

void VillagerComponent::OnUpdate(float dt) 
{
	float ms = 25.0f;
	auto& transform = GetComponent<TransformComponent>();

	// Combat
	m_time += dt;
	if(yoyo::Input::GetMouseButton(1))
	{
		if (m_time > (1.0f / attack_speed))
		{
			BasicAttack();
			m_time = 0.0f;
		}
	}

	if(yoyo::Input::GetKey(yoyo::KeyCode::Key_w))
	{
		GetComponent<psx::RigidBodyComponent>().AddForce(transform.Forward() * ms *  dt, psx::ForceMode::Impulse);
	}

	if(yoyo::Input::GetKey(yoyo::KeyCode::Key_s))
	{
		GetComponent<psx::RigidBodyComponent>().AddForce(transform.Forward() * ms *  dt * -1, psx::ForceMode::Impulse);
	}

	if(yoyo::Input::GetKey(yoyo::KeyCode::Key_d))
	{
		GetComponent<psx::RigidBodyComponent>().AddForce(yoyo::Vec3{ms, 0.0f, 0} * dt, psx::ForceMode::Impulse);
	}

	if(yoyo::Input::GetKey(yoyo::KeyCode::Key_a))
	{
		GetComponent<psx::RigidBodyComponent>().AddForce(yoyo::Vec3{-ms, 0.0f, 0} * dt, psx::ForceMode::Impulse);
	}
}

void VillagerComponent::BasicAttack()
{
	const auto& transfrom = GetComponent <TransformComponent>();

	float bullet_speed = 100.0f;
	Entity bullet = Instantiate("bullet", transfrom.position + transfrom.Forward() * 2.0f);
	MeshRendererComponent& mesh_renderer = bullet.AddComponent<MeshRendererComponent>("cubeCube0", "default_instanced_material");

	psx::RigidBodyComponent& rb = bullet.AddComponent<psx::RigidBodyComponent>();
	psx::BoxColliderComponent& col = bullet.AddComponent<psx::BoxColliderComponent>();
	rb.AddForce(transfrom.Forward() * bullet_speed, psx::ForceMode::Impulse);
}
