#pragma once

#include "NativeScript.h"

class Turret : public ScriptableEntity
{
public:
    Turret(Entity e);
    virtual ~Turret();

    virtual void OnStart() override;
    virtual void OnUpdate(float dt) override;
private:
    float m_time_elapsed = 0.0f;
    float m_attack_rate = 1.0f;
};
