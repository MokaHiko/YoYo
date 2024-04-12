#pragma once

#include "ECS/Entity.h"
#include "ECS/EntityImpl.h"
#include "ECS/Components/Components.h"

#include "Physics/PhysicsEvents.h"

namespace psx
{
    class PhysicsWorld;
    struct Collision;
    struct RaycastHit;
}

class Process;

// Scriptable Entity is the base class that many script components 
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

    // Queus an entity for destruction
    void DestroyObject(Entity e);

    // Queues attached entity for destruction
    void QueueDestroy();

    // Destroys attached entity
    void Destroy();

    // Returns if current script is active
    bool IsActive() const { return m_active; }

    // Returns if current scriptable entity is queued for destruction
    bool ToDestroy() const { return m_to_destroy; }

    // Sets script to active or inactive
    void ToggleActive(bool active) { m_active = active; }

    template <typename T>
    Entity FindEntityWithComponent() { return m_entity.m_scene->FindEntityWithComponent<T>(); }

    template <typename T>
    T* FindComponentInChildren()
    {
        YASSERT(HasComponent<TransformComponent>(), "All entities must have a transform!");

        TransformComponent& transform = GetComponent<TransformComponent>();
        for (int i = 0; i < GetComponent<TransformComponent>().children_count; i++)
        {
            if (transform.children[i].HasComponent<T>())
            {
                return &transform.children[i].GetComponent<T>();
            }
        }

        return nullptr;
    }

    bool started = false;
public:
    // Scene Queries
    bool Raycast(const yoyo::Vec3& origin, const yoyo::Vec3& dir, float max_distance, psx::RaycastHit& out);
protected:
    void StartProcess(Ref<Process> process);
private:
    friend class ScriptingSystem;
    bool m_active = false;
    bool m_to_destroy = false;

    Entity m_entity = {};
    ScriptingSystem* m_scripting_system = nullptr;
    psx::PhysicsWorld* m_physics_world = nullptr;
};

