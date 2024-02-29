#include "DemoModel.h"

#include "Math/Quaternion.h"

DemoModelComponent::DemoModelComponent(Entity e) 
	:ScriptableEntity(e)
{}

DemoModelComponent::~DemoModelComponent() {}

void DemoModelComponent::OnCreate() {}

void DemoModelComponent::OnStart() {}

void DemoModelComponent::OnUpdate(float dt) 
{
	auto& transform = GetComponent<TransformComponent>();
	transform.quat_rotation = transform.quat_rotation * yoyo::QuatFromAxisAngle({ 0.0f, 0.0f, 1.0f }, yoyo::DegToRad(-10.0f) * dt);
}
