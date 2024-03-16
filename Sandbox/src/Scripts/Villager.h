#pragma once

#include "NativeScript.h"

class VillagerComponent : public ScriptableEntity
{
public:
    VillagerComponent(Entity e);
    virtual ~VillagerComponent();

    virtual void OnCreate() override;
    virtual void OnStart() override;

    virtual void OnUpdate(float dt) override;

    void BasicAttack();
    float attack_speed = 2.0f;
    float m_time = 0.0f;
private:
    float m_t = 0.0f;
    float m_movement_speed = 90.0f;
};

