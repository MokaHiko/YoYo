#pragma once

#include "ECS/Scene.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/RenderableComponents.h"

#include <Core/Layer.h>

// TODO: Remove Forward decs
class SceneGraph;
class ScriptingSystem;
class ParticleSystemManager;

namespace yoyo
{
    class RenderPacket;
    class RendererLayer;
    class Application;
}

namespace psx 
{
    class PhysicsWorld;
}

class GameLayer : public yoyo::Layer
{
public:
    LayerType(GameLayer)

    GameLayer(yoyo::Application* app);
    virtual ~GameLayer();

    virtual void OnAttach() override;
    virtual void OnDetatch() override;

    virtual void OnEnable() override;
    virtual void OnDisable() override;

    virtual void OnUpdate(float dt) override;

    Scene* GetScene() const {return m_scene;}

    void OnMeshRendererComponentCreated(entt::basic_registry<entt::entity>&, entt::entity entity);
    void OnMeshRendererComponentDestroyed(entt::basic_registry<entt::entity>&, entt::entity entity);

    void OnCameraComponentCreated(entt::basic_registry<entt::entity>&, entt::entity entity);

    void OnCameraComponentDestroyed(entt::basic_registry<entt::entity>&, entt::entity entity);

    void OnDirectionalLightComponentCreated(entt::basic_registry<entt::entity>&, entt::entity entity);

    void OnDirectionalLightComponentDestroyed(entt::basic_registry<entt::entity>&, entt::entity entity);
private:
    // Systems
    Ref<SceneGraph> m_scene_graph;
    Ref<psx::PhysicsWorld> m_physics_world;
    Ref<ScriptingSystem> m_scripting;
    Ref<ParticleSystemManager> m_particles;

    Scene* m_scene;

    bool m_rebuild_packet = true;

    yoyo::RendererLayer* m_renderer_layer;
    yoyo::Application* m_app;
};
