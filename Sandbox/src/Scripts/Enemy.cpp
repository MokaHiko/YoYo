#include "Enemy.h"

#include <Core/Assert.h>

#include "Unit.h"
#include "UnitController.h"

#include "Villager.h"

Enemy::Enemy(Entity e) 
	:ScriptableEntity(e){}

Enemy::~Enemy() {}

void Enemy::OnStart() 
{
	YASSERT(HasComponent<Unit>(), "Enemy must have unit component!");
	YASSERT(HasComponent<UnitController>(), "Enemy must have unit component!");

	m_target = FindEntityWithComponent<VillagerComponent>();
}

void Enemy::OnUpdate(float dt) 
{
	if(m_target.IsValid())
	{
		const TransformComponent& transform = GetComponent<TransformComponent>();

		yoyo::Vec3 diff = m_target.GetComponent<TransformComponent>().position - transform.position;
		if(yoyo::Length(diff) > 0.15f)
		{
			GetComponent<UnitController>().TravelTo(m_target.GetComponent<TransformComponent>().position);
		}
	}
}
