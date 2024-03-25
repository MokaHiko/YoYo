#pragma once

#include "ECS/System.h"
#include "ScriptableEntity.h"

static const int MAX_SCRIPTS = 10;

class NativeScriptComponent
{
public:
    NativeScriptComponent();
    ~NativeScriptComponent() = default;

    void AddScript(ScriptableEntity* script);
    void RemoveScript(ScriptableEntity* script);
private:
    friend class ScriptingSystem;
    std::array<ScriptableEntity*, MAX_SCRIPTS> m_scripts = {};

    int m_scripts_count = 0;
    int m_insert_index = 0;
};

class PhysicsWorld;
class ScriptingSystem : public System<NativeScriptComponent>
{
public:
    ScriptingSystem(Scene* scene, PhysicsWorld* physics_world = nullptr /*If not passed physics event callbacks will not be called */)
        :System<NativeScriptComponent>(scene), m_physics_world(physics_world){}
    virtual ~ScriptingSystem() = default;

    virtual void Init() override;
    virtual void Shutdown() override;
    virtual void Update(float dt) override;

    virtual void OnComponentCreated(Entity e, NativeScriptComponent& native_script) override;
    virtual void OnComponentDestroyed(Entity e, NativeScriptComponent& native_script) override;

    void OnScriptCreatedCallback(ScriptableEntity* script);
    void OnScriptDestroyedCallback(ScriptableEntity* script);
private:
    void OnCollisionCallback(psx::Collision& col);
private:
    PhysicsWorld* m_physics_world;
    // TODO: Script cache
    std::vector<ScriptableEntity*> m_script_cache;
};
