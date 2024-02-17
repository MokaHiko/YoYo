#pragma once

#include "NativeScript.h"

class CameraControllerComponent : public ScriptableEntity
{
public:
    CameraControllerComponent(Entity e);
    virtual ~CameraControllerComponent();

    virtual void OnCreate() override;
    virtual void OnStart() override;

    virtual void OnUpdate(float dt) override;
private:
    float m_movement_speed = 10.0f;
};