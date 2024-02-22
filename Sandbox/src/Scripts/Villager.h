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
private:
    float m_movement_speed = 90.0f;
};

