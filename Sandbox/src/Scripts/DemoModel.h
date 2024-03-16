#pragma once

#include "ScriptableEntity.h"

class DemoModelComponent : public ScriptableEntity
{
public:
    DemoModelComponent(Entity e);
    virtual ~DemoModelComponent();

    virtual void OnCreate() override;
    virtual void OnStart() override;

    virtual void OnUpdate(float dt) override;
};
