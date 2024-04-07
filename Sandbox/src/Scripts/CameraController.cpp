#include "CameraController.h"

#include <Renderer/Camera.h>
#include <ECS/Components/RenderableComponents.h>
#include <ECS/Components/Components.h>

#include <Input/Input.h>

#include <Math/Quaternion.h>

CameraControllerComponent::CameraControllerComponent(Entity e)
	:ScriptableEntity(e) {}

CameraControllerComponent::~CameraControllerComponent() {}

void CameraControllerComponent::OnCreate() {}

void CameraControllerComponent::OnStart()
{
}

void CameraControllerComponent::OnUpdate(float dt)
{
	Ref<yoyo::Camera> camera = GetComponent<CameraComponent>().camera;
	auto& transform = GetComponent<TransformComponent>();

	if(follow && follow_target)
	{
		transform.position = follow_target.GetComponent<TransformComponent>().position;
		transform.position += follow_offset;
	}

	camera->pitch = pitch;
	camera->yaw = yaw;
	// camera->pitch = yoyo::PITCH - 30.0f;
	// camera->yaw = yoyo::YAW - 30.0f;
}

void FreeControlls(float dt)
{
	// if (yoyo::Input::GetKey(yoyo::KeyCode::Key_w))
	// {
	// 	transform.position += camera->Front() * m_movement_speed * dt;
	// }

	// if (yoyo::Input::GetKey(yoyo::KeyCode::Key_s))
	// {
	// 	transform.position -= camera->Front() * m_movement_speed * dt;
	// }

	// if (yoyo::Input::GetKey(yoyo::KeyCode::Key_d))
	// {
	// 	transform.position += camera->Right() * m_movement_speed * dt;
	// }

	// if (yoyo::Input::GetKey(yoyo::KeyCode::Key_a))
	// {
	// 	transform.position -= camera->Right() * m_movement_speed * dt;
	// }

	// if (yoyo::Input::GetKey(yoyo::KeyCode::Key_Space))
	// {
	// 	transform.position += yoyo::Vec3{ 0.0f, 1.0f, 0.0f } *m_movement_speed * dt;
	// }

	// if (yoyo::Input::GetKey(yoyo::KeyCode::Key_q))
	// {
	// 	transform.position -= yoyo::Vec3{ 0.0f, 1.0f, 0.0f } *m_movement_speed * dt;
	// }
}
