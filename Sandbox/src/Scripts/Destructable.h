#pragma once

#include "NativeScript.h"

class DestructableComponent : public ScriptableEntity
{
public:
    DestructableComponent(Entity e);
    virtual ~DestructableComponent();

    virtual void OnCreate() override;
    virtual void OnStart() override;

    virtual void OnUpdate(float dt) override;
private:
    float m_movement_speed = 10.0f;
};
