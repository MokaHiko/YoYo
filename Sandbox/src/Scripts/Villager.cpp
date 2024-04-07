#include "Villager.h"

#include <Math/Quaternion.h>
#include <Input/Input.h>
#include <Renderer/Animation.h>

#include <ECS/Components/RenderableComponents.h>
#include "Process.h"

#include "CameraController.h"

#include "Unit.h"
#include "UnitController.h"

VillagerComponent::VillagerComponent(Entity e)
	:ScriptableEntity(e) {}

VillagerComponent::~VillagerComponent() {}

void VillagerComponent::OnCreate() {}

void VillagerComponent::OnStart() 
{
	FindEntityWithComponent<CameraControllerComponent>().GetComponent<CameraControllerComponent>().follow_target = GameObject();
}

void VillagerComponent::OnUpdate(float dt) 
{
	auto& transform = GetComponent<TransformComponent>();

	// Combat
	m_time += dt;
	if(yoyo::Input::GetMouseButton(1))
	{
		if (m_time > (1.0f / attack_speed))
		{
			GetComponent<UnitController>().BasicAttack();
			m_time = 0.0f;
		}
	}

	if(yoyo::Input::GetMouseButton(3))
	{
		if (m_time > (1.0f / attack_speed))
		{
			GetComponent<UnitController>().AltAttack();
			m_time = 0.0f;
		}
	}

	// Movement
	yoyo::Vec3 input = {0.0f, 0.0f, 0.0f};
	if(yoyo::Input::GetKey(yoyo::KeyCode::Key_w))
	{
		input += yoyo::Vec3{0.0f, 0.0f, 1.0f};
	}

	if(yoyo::Input::GetKey(yoyo::KeyCode::Key_s))
	{
		input += yoyo::Vec3{0.0f, 0.0f, -1.0f};
	}

	if(yoyo::Input::GetKey(yoyo::KeyCode::Key_d))
	{
		input += yoyo::Vec3{-1, 0.0f, 0};
	}

	if(yoyo::Input::GetKey(yoyo::KeyCode::Key_a))
	{
		input += yoyo::Vec3{1, 0.0f, 0};
	}

	if(Length(input) > 0)
	{
		GetComponent<UnitController>().TravelTo(transform.position + Normalize(input));
	}
}


