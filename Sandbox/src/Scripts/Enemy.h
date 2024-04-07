
#pragma once

#include "NativeScript.h"

class Enemy : public ScriptableEntity
{
public:
    Enemy(Entity e);
    virtual ~Enemy();

    virtual void OnStart() override;
    virtual void OnUpdate(float dt) override;
private:
    Entity m_target = {};
};