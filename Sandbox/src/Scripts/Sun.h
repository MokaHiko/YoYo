#pragma once

#include "NativeScript.h"

enum class SunState
{
	Setting,
	Rising
};

class SunComponent : public ScriptableEntity
{
public:
    SunComponent(Entity e);
    virtual ~SunComponent();

    virtual void OnCreate() override;
    virtual void OnStart() override;

    virtual void OnUpdate(float dt) override;
private:
    float m_movement_speed = 10.0f;
	SunState m_state;
};

