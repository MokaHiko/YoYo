#pragma once

#include "ECS/Scene.h"
#include "ECS/Components/Components.h"

// Updates transforms and relative transform components of the scene hierarchy
class SceneGraph
{
public:
    SceneGraph(Scene* scene);
    virtual ~SceneGraph();

    void Init();
    void Shutdown();

    void Update(TransformComponent& root, float dt);
private:
    void OnTransformCreated(entt::basic_registry<entt::entity>&, entt::entity entity);
    void OnTransformDestroyed(entt::basic_registry<entt::entity>&, entt::entity entity);

    // Models are hierarchies of meshes
    void OnModelRendererCreated(entt::basic_registry<entt::entity>&, entt::entity entity);
    void RecursiveUpdate(TransformComponent& node);

    Scene* m_scene = nullptr;
};