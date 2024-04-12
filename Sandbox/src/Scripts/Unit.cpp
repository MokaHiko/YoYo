#include "Unit.h"

#include <Renderer/Material.h>

#include "ECS/Components/RenderableComponents.h"
#include "Process.h"

Unit::Unit(Entity e)
	:ScriptableEntity(e) {}

Unit::~Unit() {}

void Unit::TakeDamage(float damage, DamageType type)
{
	switch (type)
	{
	case(DamageType::Pure):
	{
		// Pure damage effect
		m_health -= damage;

		// Pure damage view effect
		MeshRendererComponent* mesh_renderer;
		if (m_view.TryGetComponent<MeshRendererComponent>(&mesh_renderer))
		{
			yoyo::Vec4 color = yoyo::Vec4{ 1.0f, 0.0f, 0.0f, 1.0f };
			mesh_renderer->material->SetProperty("diffuse_color", color.elements);
		}

		// Queue reset 
		if(!reset_pure_damage_effect_process)
		{
			reset_pure_damage_effect_process = CreateRef<DelayProcess>(0.15f, [&](){
				MeshRendererComponent* mesh_renderer;
				if (m_view.TryGetComponent<MeshRendererComponent>(&mesh_renderer))
				{
					yoyo::Vec4 color = yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
					mesh_renderer->material->SetProperty("diffuse_color", color.elements);
				}

				reset_pure_damage_effect_process.reset();
				reset_pure_damage_effect_process = nullptr;
			});

			StartProcess(reset_pure_damage_effect_process);
		}
	}break;
	default:
		YASSERT(0, "Uknown Damage Type!");
		break;
	}

	if(m_health <= 0.0f)
	{
		Die();
	}
}

void Unit::OnStart()
{
	YASSERT(HasComponent<psx::RigidBodyComponent>(), "Unit as no rigidbody!");
	GetComponent<psx::RigidBodyComponent>().LockRotationAxis({1.0f, 1.0f, 1.0f});
	GetComponent<psx::RigidBodyComponent>().SetMaxLinearVelocity(GetComponent<Unit>().GetMovementStats().ms);

	TransformComponent& transform = GetComponent<TransformComponent>();
	for (int i = 0; i < GetComponent<TransformComponent>().children_count; i++)
	{
		if (transform.children[i].HasComponent<MeshRendererComponent>())
		{
			m_view = transform.children[i];
		}
	}
	YASSERT(m_view, "Unit as not view [MeshRendere]!");
}

void Unit::OnUpdate(float dt) 
{
	if(IsStatusEffect(StatusEffect::Dead))
	{
		return;
	}
}

const bool Unit::IsStatusEffect(StatusEffect effect_flags) const
{
	return (m_status_effect & effect_flags) == effect_flags;
}

void Unit::Die() 
{
	if(IsStatusEffect(StatusEffect::Dead))
	{
		return;
	}
	m_status_effect |= StatusEffect::Dead;

	YINFO("Unit just died!");

	YASSERT(!death_process, "Cannot start death process more than once!");
	death_process = CreateRef<DelayProcess>(2.0f, [&](){
		// Despawn
		QueueDestroy();
	});
	StartProcess(death_process);
}
