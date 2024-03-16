#pragma once

#include "Scene.h"
#include <Core/Memory.h>
#include <Core/Assert.h>

// Systems operate on a scene of component type T
template<typename T>
class System
{
public:
    System(Scene* scene)
        :m_scene(scene)
    {
        m_scene->Registry().on_construct<T>().connect<&System::TCreated>(this);
        m_scene->Registry().on_destroy<T>().connect<&System::TDestroyed>(this);
    }
    virtual ~System() {};

    Scene* GetScene() { YASSERT(m_scene, "System has invalid scene!"); return m_scene; } // Get the scene this sytem operates on

    virtual void Init() {};
    virtual void Shutdown() {};
    virtual void Update(float dt) {};

    virtual void OnComponentCreated(Entity e, T& component) {};
    virtual void OnComponentDestroyed(Entity e, T& component) {};

    void AddSubsystem(Ref<System> system) {};
protected:
    // Used in the editor layer to draw the componet in the inspector.
    virtual void InspectorPanelDraw(T& component) {};
private:
    void TCreated(entt::basic_registry<entt::entity>&, entt::entity entity)
    {
        Entity e(entity, m_scene);

        T& component = e.GetComponent<T>();
        OnComponentCreated(e, component);
    }

    void TDestroyed(entt::basic_registry<entt::entity>&, entt::entity entity)
    {
        Entity e(entity, m_scene);

        T& component = e.GetComponent<T>();
        OnComponentDestroyed(e, component);
    }
private:
    Scene* m_scene;
};

// TODO: Subsystem for system dependency
// template<typename super, typename T>
// class SubSystem : public System
// {
// public:
//     SubSystem(Scene* scene, T& super_system);
// private:
// };
