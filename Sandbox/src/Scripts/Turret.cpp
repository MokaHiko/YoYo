#include "Turret.h"

#include <Math/Math.h>
#include <Math/MatrixTransform.h>

#include "ECS/Components/RenderableComponents.h"
#include "ParticleSystem/Particles.h"

#include "Projectile.h"
#include "Process.h"

Turret::Turret(Entity e)
	:ScriptableEntity(e) {}

Turret::~Turret() {}

static std::vector<yoyo::Vec3> positions = {};

void Turret::OnStart()
{
	float theta = 0.0f;
	float radians = 0.0f;

	positions.resize(18);
	for(yoyo::Vec3& pos : positions)
	{
		pos = yoyo::Vec3{yoyo::Cos(radians), 0.0f, yoyo::Sin(radians)};
		radians += yoyo::DegToRad(20);
	}

	// Ref<DelayProcess> life_time_delay_process = CreateRef<DelayProcess>(5.0f, [&](){
	// 	QueueDestroy();
	// });
	// StartProcess(life_time_delay_process);
	GameObject().AddComponent<ParticleSystemComponent>();
}

void Turret::OnUpdate(float dt) {
	return;

	m_time_elapsed += dt;
	if (m_time_elapsed > (1.0f / m_attack_rate))
	{
		for (const yoyo::Vec3& position : positions)
		{
			const auto& transform = GetComponent<TransformComponent>();
			const yoyo::Vec3& fire_point = transform.position + (position * 3.0f);

			float bullet_speed = 30.0f;
			yoyo::Mat4x4 transform_matrix = yoyo::TranslationMat4x4(fire_point) * yoyo::ScaleMat4x4({ 0.5f, 0.5f, 0.5f });

			Entity bullet = Instantiate("bullet", transform_matrix);
			MeshRendererComponent& mesh_renderer = bullet.AddComponent<MeshRendererComponent>("Cube", "grenade_instanced_material");

			psx::RigidBodyComponent& rb = bullet.AddComponent<psx::RigidBodyComponent>();
			rb.SetUseGravity(false);
			rb.LockRotationAxis({ 1, 1, 1 });

			psx::BoxColliderComponent& col = bullet.AddComponent<psx::BoxColliderComponent>(yoyo::Vec3{ 0.5f, 0.5f, 0.5f });

			bullet.AddComponent<Projectile>(bullet);
			yoyo::Vec3 impulse = position * bullet_speed;
			rb.AddForce(impulse, psx::ForceMode::Impulse);
		}

		m_time_elapsed = 0.0f;
	}
}
