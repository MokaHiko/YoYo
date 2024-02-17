#include "Destructable.h"

DestructableComponent::DestructableComponent(Entity e) 
	: ScriptableEntity(e){}

DestructableComponent::~DestructableComponent() {}

void DestructableComponent::OnCreate() 
{
}

void DestructableComponent::OnStart() 
{
}

void DestructableComponent::OnUpdate(float dt) 
{
	auto& transform = GetComponent<TransformComponent>();
	transform.rotation += yoyo::Normalize(transform.rotation) *dt;
}
