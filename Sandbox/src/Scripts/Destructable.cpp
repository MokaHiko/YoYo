#include "Destructable.h"

#include <Physics/PhysicsTypes.h>
#include <Input/Input.h>

#include <Math/Random.h>

DestructableComponent::DestructableComponent(Entity e) 
	: ScriptableEntity(e){}

DestructableComponent::~DestructableComponent() {}

void DestructableComponent::OnCreate() 
{
}

void DestructableComponent::OnStart() 
{
}

static yoyo::PRNGenerator<float> random(-2, 2.0f);

void DestructableComponent::OnUpdate(float dt) 
{
	static float timer = 0.0f;
	timer += dt;

	if(timer > 2.0f)
	{
		QueueDestroy();
	}

	// if(yoyo::Input::GetKey(yoyo::KeyCode::Key_j))
	// {
	// 	psx::RigidBodyComponent& rb = GetComponent<psx::RigidBodyComponent>();
	// 	rb.AddForce({random.Next(), random.Next(), random.Next()}, psx::ForceMode::Impulse);
	// }
}

void DestructableComponent::OnCollisionEnter(const psx::Collision& col)
{
	// Entity other(col.b, GetScene());
	// const std::string& other_tag = other.GetComponent<TagComponent>().tag;

	// const std::string& my_tag = GetComponent<TagComponent>().tag;
	// YINFO("%s hit %s", my_tag.c_str(), other_tag.c_str());
}
