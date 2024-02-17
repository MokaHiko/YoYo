#pragma once

#include "ECS/Entity.h"
#include "ECS/EntityImpl.h"
#include "ECS/Components/Components.h"

// Scriptable Entity is the base class that many Unity scripts derive from.
// class Process;
class ScriptableEntity
{
public:
    ScriptableEntity(Entity e);
    virtual ~ScriptableEntity();

    virtual void OnCreate() {}
    virtual void OnStart() {}
    virtual void OnUpdate(float dt) {}

    virtual void OnCollisionEnter2D(Entity other) {}

    // Gets reference to attached entity's component 
    template <typename T>
    T& GetComponent() { return m_entity.GetComponent<T>(); }

    // Insantiates new entity
    Entity Instantiate(const std::string& name = "");

    // Queues attached entity for destruction
    // void QueueDestroy() { m_entity.m_scene->QueueDestroy(m_entity);}
private:
    //void StartProcess(Ref<Process> process);
    Entity m_entity = {};
};