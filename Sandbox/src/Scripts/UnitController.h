#pragma once

#include <Math/Math.h>
#include "NativeScript.h"

namespace yoyo
{
    class Animator;
};

class AnimateTransformProcess;
class Unit;
class UnitController : public ScriptableEntity
{
public:
    UnitController(Entity e);
    virtual ~UnitController();
public:
    // Combat
    void BasicAttack();
    void AltAttack();

    float attack_speed = 25.0f;
    float m_time = 0.0f;
public:
    // Movement
    void TravelTo(const yoyo::Vec3& target_position);
public:
    //virtual void OnCreate() override;
    virtual void OnStart() override;
    virtual void OnUpdate(float dt) override;
private:
    yoyo::Vec3 m_target_position = {0.0f, 0.0f, 0.0f};
    Ref<AnimateTransformProcess> m_animate_transform_process;
private:
    Entity m_view = {};
    Ref<yoyo::Animator> m_animator = nullptr;
};
