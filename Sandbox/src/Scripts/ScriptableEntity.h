#pragma once

#include "ECS/Entity.h"
#include "ECS/EntityImpl.h"
#include "ECS/Components/Components.h"

#include "Physics/PhysicsEvents.h"

// Scriptable Entity is the base class that many script components 
struct psx::Collision;
class ScriptableEntity
{
public:
    ScriptableEntity(Entity e);
    virtual ~ScriptableEntity();

    virtual void OnCreate() {}
    virtual void OnStart() {}

    virtual void OnUpdate(float dt) {}

    virtual void OnDestroy() {}

    virtual void OnCollisionEnter(const psx::Collision& col) {}

    // Gets entity's scene 
    Scene* GetScene() const { return m_entity.m_scene; }

    // Gets the entity 
    Entity GameObject();

    // Returns whether or not attached entity has component
    template <typename T>
    bool HasComponent() const { return m_entity.HasComponent<T>(); }

    // Gets reference to attached entity's component 
    template <typename T>
    T& GetComponent() { return m_entity.GetComponent<T>(); }

    // Insantiates new entity
    Entity Instantiate(const std::string& name = "", const yoyo::Vec3& position = {});
    Entity Instantiate(const std::string& name, const yoyo::Mat4x4& transform);

    // Queues attached entity for destruction
    void QueueDestroy();

    // Returns if current script is active
    bool IsActive() const { return m_active; }

    // Sets script to active or inactive
    void ToggleActive(bool active) { m_active = active; }

    template <typename T>
    Entity FindEntityWithComponent() {return m_entity.m_scene->FindEntityWithComponent<T>();}

    bool started = false;
protected:
    class Process;
    //void StartProcess(Ref<Process> process);
private:
    bool m_active = false;
    bool m_to_destroy = false;
    Entity m_entity = {};
};
