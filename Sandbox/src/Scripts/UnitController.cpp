#include "UnitController.h"

#include <Core/Assert.h>
#include <Math/Random.h>
#include <Math/MatrixTransform.h>
#include <Renderer/Animation.h>

#include "ECS/Components/RenderableComponents.h"
#include "Process.h"

#include "Unit.h"
#include "Projectile.h"
#include "Turret.h"

class AnimateTransformProcess : public Process
{
public:
	AnimateTransformProcess(Entity e, const yoyo::Vec3& t_pos, const yoyo::Quat& t_rot, const yoyo::Vec3& t_scale, float duration)
	{
		if (!e.IsValid())
		{
			YWARN("AnimateTransformProcess: Entity handle is null. Failing!");
			Fail();
			return;
		}

		m_entity = e;
		m_duration = duration;

		start_position = e.GetComponent<TransformComponent>().position;
		target_position = t_pos;

		start_rotation = e.GetComponent<TransformComponent>().quat_rotation;
		target_rotation = t_rot;

		start_scale = e.GetComponent<TransformComponent>().scale;
		target_scale = t_scale;
	}
	virtual ~AnimateTransformProcess() = default;

	virtual void OnUpdate(float dt) override
	{
		if (!m_entity.IsValid())
		{
			YWARN("AnimateTransformProcess: Entity handle is null. Failing!");
			Fail();
			return;
		}

		m_time_elapsed += dt;
		if (m_time_elapsed >= m_duration)
		{
			Succeed();
		}
		TransformComponent& transform = m_entity.GetComponent<TransformComponent>();

		transform.position = yoyo::Lerp(start_position, target_position, m_time_elapsed / m_duration);
		transform.quat_rotation = yoyo::Slerp(start_rotation, target_rotation, m_time_elapsed / m_duration);
		transform.scale = yoyo::Lerp(start_scale, target_scale, m_time_elapsed / m_duration);
	};

	// Process end callbacks
	virtual void OnSuccess(){}

	virtual void OnFail() {}

	virtual void OnAbort() {}
private:
	Entity m_entity;

	yoyo::Vec3 start_position;
	yoyo::Vec3 target_position;

	yoyo::Vec3 start_scale;
	yoyo::Vec3 target_scale;

	yoyo::Quat start_rotation;
	yoyo::Quat target_rotation;

	float m_time_elapsed = 0.0f;
	float m_duration = 0.5f;
};

static yoyo::PRNGenerator<float> pos_generator(-100.0f, 100.0f);

UnitController::UnitController(Entity e)
	:ScriptableEntity(e) {}

UnitController::~UnitController() {}

void UnitController::OnStart()
{
	if (!HasComponent<Unit>())
	{
		YERROR("UnitController disabled : cannot be used without <Unit> script component!");
		ToggleActive(false);
		return;
	}

	if (!HasComponent<psx::RigidBodyComponent>())
	{
		YERROR("UnitController disabled : cannot be used without Rigidbody component!");
		ToggleActive(false);
		return;
	}

	TransformComponent& transform = GetComponent<TransformComponent>();
	for (int i = 0; i < GetComponent<TransformComponent>().children_count; i++)
	{
		if (transform.children[i].HasComponent<MeshRendererComponent>())
		{
			m_view = transform.children[i];

			AnimatorComponent* animator_component;
			if (m_view.TryGetComponent<AnimatorComponent>(&animator_component))
			{
				m_animator = animator_component->animator;
			}
			else
			{
				YWARN("UnitController: Animations disabled. No animator in view!");
			}
		}
	}

	TravelTo(transform.position);
}

void UnitController::TravelTo(const yoyo::Vec3& target_position)
{
	m_target_position = target_position;
}

void UnitController::OnUpdate(float dt)
{
	if (!HasComponent<Unit>())
	{
		YERROR("UnitController disabled : cannot be used without <Unit> script component!");
		ToggleActive(false);
		return;
	}

	auto& transform = GetComponent<TransformComponent>();

	// Movement
	yoyo::Vec3 diff = m_target_position - transform.position;
	if (yoyo::Length(diff) > 0.15f)
	{
		const UnitMovementStats& movement_stats = GetComponent<Unit>().GetMovementStats();
		yoyo::Vec3 dir = yoyo::Normalize(diff);

		GetComponent<psx::RigidBodyComponent>().AddForce(dir * movement_stats.agility * dt, psx::ForceMode::Impulse);

		// Process View
		YASSERT(m_view, "View has is null entity!");
		TransformComponent& view_transform = m_view.GetComponent<TransformComponent>();

		// Transform Animations
		// TODO: Make member
		static float last_rot = 0;

		// Rotation
		yoyo::Vec3 normalized_delta = dir;
		float dot = yoyo::Dot({ 0.0f, 0.0f, 1.0f }, normalized_delta);
		float rot = yoyo::ACos(dot);

		// Correct direction
		if (dir.x < 0)
		{
			rot *= -1.0f;
		}

		// Check if new rotation
		if(rot != last_rot)
		{
			if (m_animate_transform_process && m_animate_transform_process->IsAlive())
			{
				m_animate_transform_process->Abort();
			}

			yoyo::Quat target_rotation = yoyo::QuatFromAxisAngle(yoyo::Vec3{ 0.0f, 1.0f, 0.0f }, rot, true);
			m_animate_transform_process = CreateRef<AnimateTransformProcess>(m_view, view_transform.position, target_rotation, view_transform.scale, 0.15f);
			StartProcess(m_animate_transform_process);

			last_rot = rot;
		}

		// Animations
		AnimatorComponent* animator_component;
		if (m_view.TryGetComponent<AnimatorComponent>(&animator_component))
		{
			animator_component->animator->Play(0);
		}
	}
	else
	{
		GetComponent<psx::RigidBodyComponent>().SetLinearVelocity({ 0.0f });
		m_target_position = transform.position;

		// View
		 YASSERT(m_view, "View has is null entity!");

		 //Animations
		 AnimatorComponent* animator_component;
		 if (m_view.TryGetComponent<AnimatorComponent>(&animator_component))
		 {
		 	animator_component->animator->Play(1);
		 }
	}
}

void UnitController::BasicAttack()
{
	// const auto& transform = GetComponent<TransformComponent>();
	// const auto& view_transform = m_view.GetComponent<TransformComponent>();
	// const yoyo::Vec3& fire_point = transform.position + (view_transform.Forward() * 2.0f);

	// psx::RaycastHit hit;
	// if(Raycast(fire_point, view_transform.Forward(), 1000.0f, hit))
	// {
	// 	Entity e(hit.entity_id, GetScene());

	// 	Unit* unit;
	// 	if(e.TryGetComponent<Unit>(&unit))
	// 	{
	// 		unit->TakeDamage(100.0f);
	// 	}
	// }

	//Projectile
	const auto& transform = GetComponent<TransformComponent>();
	static const auto& view_transform = m_view.GetComponent<TransformComponent>();
	const yoyo::Vec3& fire_point = transform.position + (view_transform.Forward() * 3.0f);

	float bullet_speed = 60.0f;
	yoyo::Mat4x4 transform_matrix = yoyo::TranslationMat4x4(fire_point) * yoyo::ScaleMat4x4({0.5f, 0.5f, 0.5f});

	Entity bullet = Instantiate("Bullet", transform_matrix);
	MeshRendererComponent& mesh_renderer = bullet.AddComponent<MeshRendererComponent>("Cube", "grenade_instanced_material");

	psx::RigidBodyComponent& rb = bullet.AddComponent<psx::RigidBodyComponent>();
	rb.SetUseGravity(false);
	rb.LockRotationAxis({1,1,1});

	psx::BoxColliderComponent& col = bullet.AddComponent<psx::BoxColliderComponent>(yoyo::Vec3{0.5f, 0.5f, 0.5f});

	bullet.AddComponent<Projectile>(bullet);
	yoyo::Vec3 impulse = view_transform.Forward() * bullet_speed;
	rb.AddForce(impulse, psx::ForceMode::Impulse);
}

void UnitController::AltAttack()
{
	//Projectile
	const auto& transform = GetComponent<TransformComponent>();
	static const auto& view_transform = m_view.GetComponent<TransformComponent>();
	const yoyo::Vec3& fire_point = transform.position + (view_transform.Forward() * 3.0f);

	float bullet_speed = 20.0f;
	// yoyo::Mat4x4 transform_matrix = yoyo::TranslationMat4x4(transform.position + view_transform.Forward() * 2.0f) * yoyo::ScaleMat4x4({0.25f, 0.25f, 0.25f});
	yoyo::Mat4x4 transform_matrix = yoyo::TranslationMat4x4(fire_point) * yoyo::ScaleMat4x4({1.0f, 1.0f, 1.0f});

	Entity bullet = Instantiate("Turret", transform_matrix);
	MeshRendererComponent& mesh_renderer = bullet.AddComponent<MeshRendererComponent>("Cube", "grenade_instanced_material");

	psx::RigidBodyComponent& rb = bullet.AddComponent<psx::RigidBodyComponent>();
	//rb.SetUseGravity(false);
	rb.LockRotationAxis({1,1,1});

	//psx::BoxColliderComponent& col = bullet.AddComponent<psx::BoxColliderComponent>(yoyo::Vec3{0.25f, 0.25f, 0.25f});
	psx::BoxColliderComponent& col = bullet.AddComponent<psx::BoxColliderComponent>();

	//bullet.AddComponent<Projectile>(bullet);
	bullet.AddComponent<Turret>(bullet);
	yoyo::Vec3 impulse = view_transform.Forward() * bullet_speed;
	impulse.y = 20.0f;
	rb.AddForce(impulse, psx::ForceMode::Impulse);
}