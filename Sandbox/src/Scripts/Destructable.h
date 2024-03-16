#pragma once

#include "ScriptableEntity.h"

class DestructableComponent : public ScriptableEntity
{
public:
    DestructableComponent(Entity e);
    virtual ~DestructableComponent();

    virtual void OnCreate() override;
    virtual void OnStart() override;

    virtual void OnUpdate(float dt) override;

    virtual void OnCollisionEnter(const psx::Collision& col);
};
