#include "Projectile.h"
#include "Unit.h"

Projectile::Projectile(Entity e)
	:ScriptableEntity(e) {}

Projectile::~Projectile() {}

void Projectile::OnUpdate(float dt)
{
	m_time_elapsed += dt;
	if (m_time_elapsed >= m_life_time)
	{
		QueueDestroy();
	}
}

void Projectile::OnCollisionEnter(const psx::Collision& col) {
	Entity e(col.b, GetScene());

	Unit* unit;
	if (e.TryGetComponent<Unit>(&unit)) 
	{
		unit->TakeDamage(25.0f);
		QueueDestroy();
	}
}
