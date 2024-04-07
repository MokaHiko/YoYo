#pragma once

#include "ScriptableEntity.h"

class CameraControllerComponent : public ScriptableEntity
{
public:
    CameraControllerComponent(Entity e);
    virtual ~CameraControllerComponent();

    virtual void OnCreate() override;
    virtual void OnStart() override;

    virtual void OnUpdate(float dt) override;

    bool follow = true;
    Entity follow_target = {};
    yoyo::Vec3 follow_offset = {0, 25, -25};

    float pitch = -35;
    float yaw = 115.0f;
private:
    float m_movement_speed = 10.0f;
};