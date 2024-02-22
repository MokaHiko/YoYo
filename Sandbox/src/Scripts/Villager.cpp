#include "Villager.h"

VillagerComponent::VillagerComponent(Entity e)
	:ScriptableEntity(e)
{

}

VillagerComponent::~VillagerComponent()
{

}

void VillagerComponent::OnCreate() {}

void VillagerComponent::OnStart() {}

void VillagerComponent::OnUpdate(float dt) 
{
	GetComponent<TransformComponent>().rotation.y += yoyo::DegToRad(m_movement_speed * dt);
}
