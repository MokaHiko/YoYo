#pragma once

#include "NativeScript.h"

enum class DamageType
{
	Pure,
	Blunt,
};

enum class StatusEffect 
{
	Clean = 0,
	Dead = 1,
	Bleeding = 1 << 1,
	Burning = 1 << 2
};

inline StatusEffect operator~ (StatusEffect a) { return (StatusEffect)~(int)a; }
inline StatusEffect operator| (StatusEffect a, StatusEffect b) { return (StatusEffect)((int)a | (int)b); }
inline StatusEffect operator& (StatusEffect a, StatusEffect b) { return (StatusEffect)((int)a & (int)b); }
inline StatusEffect operator^ (StatusEffect a, StatusEffect b) { return (StatusEffect)((int)a ^ (int)b); }
inline StatusEffect& operator|= (StatusEffect& a, StatusEffect b) { return (StatusEffect&)((int&)a |= (int)b); }
inline StatusEffect& operator&= (StatusEffect& a, StatusEffect b) { return (StatusEffect&)((int&)a &= (int)b); }
inline StatusEffect& operator^= (StatusEffect& a, StatusEffect b) { return (StatusEffect&)((int&)a ^= (int)b); }

struct UnitMovementStats
{
    float ms = 12.5f;
    float agility = 25.0f;
};

class DelayProcess;
class Unit : public ScriptableEntity
{
public:
    Unit(Entity e);
    virtual ~Unit();

    virtual void TakeDamage(float damage, DamageType type = DamageType::Pure);
public:
    virtual void OnStart() override;
    virtual void OnUpdate(float dt) override;

public:
    // Returns whether or not unit has the status effect
    const bool IsStatusEffect(StatusEffect effect_flags) const;

    // Returns the movement stats of the unit
    const UnitMovementStats& GetMovementStats() const {return m_movement_stats;}
    UnitMovementStats& GetMovementStats() {return m_movement_stats;}
private:
    void Die();
private:
	Ref<DelayProcess> reset_pure_damage_effect_process;
	Ref<DelayProcess> death_process;
private:
    Entity m_view = {};
    float m_health = 100.0f;

    UnitMovementStats m_movement_stats = {};
	StatusEffect m_status_effect = StatusEffect::Clean;
};