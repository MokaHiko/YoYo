#pragma once

#include "NativeScript.h"

class Projectile : public ScriptableEntity
{
public:
    Projectile(Entity e);
    virtual ~Projectile();

    virtual void OnStart() override;
    virtual void OnUpdate(float dt) override;
    virtual void OnCollisionEnter(const psx::Collision& col) override;

    virtual void Die();
private:
    float m_time_elapsed = 0.0f;
    float m_life_time = 2.0f;
};


