#include "Sun.h"

#include <Renderer/Light.h>

#include "ECS/Components/RenderableComponents.h"

SunComponent::SunComponent(Entity e) 
	:ScriptableEntity(e)
{
}

SunComponent::~SunComponent() {}

void SunComponent::OnCreate() {}

void SunComponent::OnStart() {}

void SunComponent::OnUpdate(float dt) 
{
	return;
	auto& transform = GetComponent<TransformComponent>();

	// TODO: Define in sandbox scene
	static float time = 0.0f;
	static float day_duration = 5.0f;

	static const float start_position = transform.position.x;
	static const float end_position = transform.position.x + (m_movement_speed * day_duration);

	if(time > day_duration)
	{
		m_state = SunState::Setting;
	}

	if(time <= 0.0f)
	{
		m_state = SunState::Rising;
	}

	if(m_state == SunState::Rising)
	{
		time += dt;
		transform.position.x = yoyo::Lerp(start_position, end_position, time / day_duration);
	}
	else if(m_state == SunState::Setting)	
	{
		time -= dt;
		transform.position.x = yoyo::Lerp(end_position, start_position, (1 - (time / day_duration)));
	}

	transform.rotation += yoyo::Normalize(yoyo::Vec3{1.0f, 1.0f, 1.0f}) * dt;

	auto& light = GetComponent<DirectionalLightComponent>();
	light.dir_light->direction = yoyo::Vec4{transform.position.x, transform.position.y, transform.position.z, 0.0f} * -1.0f;
}
