#include <Yoyo.h>

#include <Core/Log.h>

#include <Math/Math.h>
#include <Math/MatrixTransform.h>
#include <Math/Quaternion.h>
#include <Math/Random.h>

#include <Input/Input.h>

#include <Core/Memory.h>
#include <Resource/ResourceManager.h>

#include <Renderer/RendererLayer.h>

#include "ECS/Scene.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/RenderableComponents.h"

#include "Scripts/CameraController.h"
#include "Scripts/Destructable.h"
#include "Scripts/Sun.h"
#include "Scripts/Villager.h"
#include "Scripts/DemoModel.h"

#include "SceneGraph/SceneGraph.h"

#include "Physics/Physics3D.h"

class GameLayer : public yoyo::Layer
{
public:
    GameLayer(yoyo::Application& app)
        :m_app(app) {}

    virtual ~GameLayer() {}

    virtual void OnEnable() override
    {
        // Init systems
        m_scene = Y_NEW Scene();

        // TODO: Place in each system
        {
            m_scene->Registry().on_construct<CameraComponent>().connect<&GameLayer::OnCameraComponentCreated>(this);
            m_scene->Registry().on_destroy<CameraComponent>().connect<&GameLayer::OnCameraComponentDestroyed>(this);

            m_scene->Registry().on_construct<DirectionalLightComponent>().connect<&GameLayer::OnDirectionalLightComponentCreated>(this);
            m_scene->Registry().on_destroy<DirectionalLightComponent>().connect<&GameLayer::OnDirectionalLightComponentDestroyed>(this);

            m_scene->Registry().on_construct<MeshRendererComponent>().connect<&GameLayer::OnMeshRendererComponentCreated>(this);
            m_scene->Registry().on_destroy<MeshRendererComponent>().connect<&GameLayer::OnMeshRendererComponentDestroyed>(this);

            m_scene_graph = CreateRef<SceneGraph>(m_scene);
            m_scene_graph->Init();

            m_physics_world = CreateRef<psx::PhysicsWorld>(m_scene);
            m_physics_world->Init();
        }

        m_renderer_layer = m_app.FindLayer<yoyo::RendererLayer>();
        m_rp = Y_NEW yoyo::RenderPacket;

        yoyo::Model::LoadFromAsset("assets/models/cube.yo");

        // Set up scene
        auto camera = m_scene->Instantiate("camera", { 0.0f, 5.0f, 20.0f });
        camera.AddComponent<CameraComponent>();
        camera.AddComponent<CameraControllerComponent>(camera);

        Ref<yoyo::Shader> default_lit = yoyo::ResourceManager::Instance().Load<yoyo::Shader>("lit_shader");
        {
            Ref<yoyo::Material> light_material = yoyo::Material::Create(default_lit, "light_material");
            light_material->receive_shadows = false;

            light_material->SetTexture(yoyo::MaterialTextureType::MainTexture, yoyo::ResourceManager::Instance().Load<yoyo::Texture>("assets/textures/container2.yo"));

            // TODO: Apply material color
            light_material->color = yoyo::Vec4{ 1.0f, 1.0f, 0.0f, 1.0f };
            light_material->SetVec4("diffuse_color", yoyo::Vec4{ 1.0f, 1.0f, 0.0f, 1.0f });
            light_material->SetVec4("specular_color", yoyo::Vec4{ 0.25, 0.0f, 0.0f, 1.0f });

            Entity light = m_scene->Instantiate("light", { 100.0f, 60.0f, 5.0f });
            TransformComponent& transform = light.GetComponent<TransformComponent>();

            Ref<yoyo::DirectionalLight> dir_light = light.AddComponent<DirectionalLightComponent>().dir_light;
            dir_light->color = { 1.0f, 1.0f, 1.0f, 1.0f };
            dir_light->direction = yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f } *-1.0f;

            auto& mesh_renderer = light.AddComponent<MeshRendererComponent>();
            mesh_renderer.mesh = yoyo::ResourceManager::Instance().Load<yoyo::Mesh>("cubeCube0");
            mesh_renderer.material = light_material;

            light.AddComponent<SunComponent>(light);
        }


        Ref<yoyo::Shader> default_lit_instanced = yoyo::ResourceManager::Instance().Load<yoyo::Shader>("lit_instanced_shader");
        {
            Ref<yoyo::Material> crate_material = yoyo::Material::Create(default_lit_instanced, "crate_material");
            crate_material->SetTexture(yoyo::MaterialTextureType::MainTexture, yoyo::ResourceManager::Instance().Load<yoyo::Texture>("assets/textures/container2.yo"));
            crate_material->color = yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
            crate_material->SetVec4("diffuse_color", yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
            crate_material->SetVec4("specular_color", yoyo::Vec4{ 0.25, 0.0f, 0.0f, 1.0f });

            yoyo::PRNGenerator<float> angle_generator(0.0f, 360.0f);
            yoyo::PRNGenerator<float> pos_generator(-10.0f, 10.0f);
            yoyo::PRNGenerator<float> height_generator(2.0f, 15.0f);

            for (int i = 0; i < 0; i++)
            {
                auto crate = m_scene->Instantiate("crate", { pos_generator.Next(), height_generator.Next(), pos_generator.Next() });

                TransformComponent& transform = crate.GetComponent<TransformComponent>();
                transform.rotation = { angle_generator.Next(), angle_generator.Next(), angle_generator.Next() };

                auto& mesh_renderer = crate.AddComponent<MeshRendererComponent>();
                mesh_renderer.mesh = yoyo::ResourceManager::Instance().Load<yoyo::Mesh>("cubeCube0");
                mesh_renderer.material = crate_material;

                crate.AddComponent<DestructableComponent>(crate);
                crate.AddComponent<psx::RigidBodyComponent>();
            }
        }

        // Load claptraps
        {
            auto claptrap_model = yoyo::Model::LoadFromAsset("assets/models/claptrap.yo");
            auto claptrap_material = yoyo::Material::Create(default_lit_instanced, "claptrap_material");
            claptrap_material->SetTexture(yoyo::MaterialTextureType::MainTexture, yoyo::ResourceManager::Instance().Load<yoyo::Texture>("assets/textures/Claptrap_blender_fix_Claptrap_BaseColor.yo"));
            claptrap_material->SetTexture(yoyo::MaterialTextureType::SpecularMap, yoyo::ResourceManager::Instance().Load<yoyo::Texture>("assets/textures/Claptrap_blender_fix_Claptrap_Roughness.yo"));

            claptrap_material->color = yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
            claptrap_material->SetVec4("diffuse_color", yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
            claptrap_material->SetVec4("specular_color", yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

            yoyo::PRNGenerator<float> angle_generator(0.0f, 360.0f);
            yoyo::PRNGenerator<float> pos_generator(-10.0f, 10.0f);
            yoyo::PRNGenerator<float> height_generator(2.0f, 15.0f);

            for (int i = 0; i < 100; i++)
            {
                auto claptrap = m_scene->Instantiate("1claptrap" + std::to_string(i), {pos_generator.Next(), height_generator.Next(), pos_generator.Next()});
                TransformComponent& model_transform = claptrap.GetComponent<TransformComponent>();
                
                for (int j = 0; j < claptrap_model->meshes.size(); j++)
                {
                    Entity mesh = m_scene->Instantiate("claptrap" + std::to_string(j + i));
                    TransformComponent& transform = mesh.GetComponent<TransformComponent>();

                    auto& mesh_renderer = mesh.AddComponent<MeshRendererComponent>();
                    mesh_renderer.mesh = claptrap_model->meshes[j];
                    mesh_renderer.material = claptrap_material;

                    model_transform.AddChild(mesh);
                }
                model_transform.scale *= 0.1f;
                model_transform.quat_rotation = yoyo::QuatFromAxisAngle({ 1.0f, 0.0f, 0.0f }, yoyo::DegToRad(-90.0f)) *
                                                yoyo::QuatFromAxisAngle({ 0.0f, 0.0f, 1.0f }, yoyo::DegToRad(90.0f));

                claptrap.AddComponent<psx::RigidBodyComponent>();
            }
        }

        {
            Ref<yoyo::Material> green_material = yoyo::Material::Create(default_lit, "green_material");
            green_material->SetTexture(yoyo::MaterialTextureType::MainTexture, yoyo::ResourceManager::Instance().Load<yoyo::Texture>("assets/textures/white.yo"));
            green_material->color = yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
            green_material->SetVec4("diffuse_color", yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
            green_material->SetVec4("specular_color", yoyo::Vec4{ 0.25, 0.0f, 0.0f, 1.0f });

            auto plane = m_scene->Instantiate("plane", { 0.0f, -1.0f, 0.0f });
            plane.GetComponent<TransformComponent>().scale = { 1000.0f, 1.0f, 1000.0f };
            auto& mesh_renderer = plane.AddComponent<MeshRendererComponent>();
            mesh_renderer.mesh = yoyo::ResourceManager::Instance().Load<yoyo::Mesh>("cubeCube0");
            mesh_renderer.material = green_material;
        }
    }

    virtual void OnDisable() override
    {
        // Shutdown Systems
        m_physics_world->Shutdown();
        m_scene_graph->Shutdown();

        // Clean up handles
        delete m_rp;
        delete m_scene;
    };

    virtual void OnUpdate(float dt) override
    {
        // Physics System
        m_physics_world->Update(dt);

        // Scene Graph
        m_scene_graph->Update(dt);

        for (auto& id : m_scene->Registry().view<TransformComponent, MeshRendererComponent>())
        {
            Entity e{ id, m_scene };
            TransformComponent& transform = e.GetComponent<TransformComponent>();

            // TODO: Move to renderable 
            auto& mesh_renderer = e.GetComponent<MeshRendererComponent>();
            mesh_renderer.mesh_object->model_matrix = transform.model_matrix;
        }

        // Scripting System
        {
            for (auto& id : m_scene->Registry().view<TransformComponent, CameraControllerComponent>())
            {
                Entity e{ id, m_scene };
                auto& camera = e.GetComponent<CameraControllerComponent>();

                // Update Render Scene
                camera.OnUpdate(dt);
            }

            for (auto& id : m_scene->Registry().view<TransformComponent, DestructableComponent>())
            {
                Entity e{ id, m_scene };
                auto& destructable = e.GetComponent<DestructableComponent>();

                // Update Render Scene
                destructable.OnUpdate(dt);
            }

            for (auto& id : m_scene->Registry().view<TransformComponent, SunComponent>())
            {
                Entity e{ id, m_scene };
                auto& sun = e.GetComponent<SunComponent>();

                // Update Render Scene
                sun.OnUpdate(dt);
            }

            for (auto& id : m_scene->Registry().view<TransformComponent, VillagerComponent>())
            {
                Entity e{ id, m_scene };
                auto& villager = e.GetComponent<VillagerComponent>();

                // Update Render Scene
                villager.OnUpdate(dt);
            }

            for (auto& id : m_scene->Registry().view<TransformComponent, DemoModelComponent>())
            {
                Entity e{ id, m_scene };
                auto& demo = e.GetComponent<DemoModelComponent>();

                // Update Render Scene
                demo.OnUpdate(dt);
            }
        }

        // Update camera matrices
        for (auto& id : m_scene->Registry().view<TransformComponent, CameraComponent>())
        {
            Entity e{ id, m_scene };

            Ref<yoyo::Camera> cam = e.GetComponent<CameraComponent>().camera;
            cam->position = e.GetComponent<TransformComponent>().position;
            cam->UpdateCameraVectors();
        }

        // Lights
        for (auto& id : m_scene->Registry().view<TransformComponent, DirectionalLightComponent>())
        {
            Entity e{ id, m_scene };

            TransformComponent& transform = e.GetComponent<TransformComponent>();
            Ref<yoyo::DirectionalLight> dir_light = e.GetComponent<DirectionalLightComponent>().dir_light;

            // TODO: calculate base of transform
            // yoyo::Vec3 front = Normalize(dir_light.direction);
            // yoyo::Vec3 right = Normalize(Cross(front, { 0.0f, 1.0f, 0.0f }));
            // yoyo::Vec3 up = Normalize(Cross(right, front));

            const yoyo::ApplicationSettings& settings = m_app.Settings();
            float width = 16 * 4.0f;
            float height = 9 * 4.0f;
            float half_width = static_cast<float>(width);
            float half_height = static_cast<float>(height);

            yoyo::Mat4x4 proj = yoyo::OrthographicProjectionMat4x4(-half_width, half_width, -half_height, half_height, -1000, 1000);
            proj[5] *= -1.0f;
            dir_light->view_proj = proj * yoyo::LookAtMat4x4(transform.position, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        }

        // Rendering system (send render packets)
        m_rp->Clear();
        if (m_rebuild_packet)
        {
            // Cameras
            for (auto& id : m_scene->Registry().view<NewCameraComponent, CameraComponent>())
            {
                Entity e{ id, m_scene };

                Ref<yoyo::Camera> cam = e.GetComponent<CameraComponent>().camera;
                cam->position = e.GetComponent<TransformComponent>().position;
                cam->UpdateCameraVectors();

                m_rp->new_camera = cam;

                e.RemoveComponent<NewCameraComponent>();
                break;
            }

            // Lights
            for (auto& id : m_scene->Registry().view<NewDirectionalLightComponent, DirectionalLightComponent>())
            {
                Entity e{ id, m_scene };

                TransformComponent& transform = e.GetComponent<TransformComponent>();
                Ref<yoyo::DirectionalLight> dir_light = e.GetComponent<DirectionalLightComponent>().dir_light;

                m_rp->new_dir_lights.emplace_back(dir_light);
                e.RemoveComponent<NewDirectionalLightComponent>();
            }

            // Meshes
            for (auto& id : m_scene->Registry().view<NewMeshComponent, MeshRendererComponent>())
            {
                Entity e{ id, m_scene };

                TransformComponent& transform = e.GetComponent<TransformComponent>();

                MeshRendererComponent& mesh_renderer = e.GetComponent<MeshRendererComponent>();
                mesh_renderer.mesh_object->mesh = mesh_renderer.mesh;
                mesh_renderer.mesh_object->material = mesh_renderer.material;
                mesh_renderer.mesh_object->model_matrix = transform.model_matrix;

                // Update Render Packet
                m_rp->new_objects.emplace_back(mesh_renderer.mesh_object);
                e.RemoveComponent<NewMeshComponent>();
            }

            m_rebuild_packet = false;
        }
        m_renderer_layer->SendRenderPacket(m_rp);
    };

    void OnMeshRendererComponentCreated(entt::basic_registry<entt::entity>&, entt::entity entity)
    {
        Entity e(entity, m_scene);
        e.GetComponent<MeshRendererComponent>().mesh_object = CreateRef<yoyo::MeshPassObject>();

        e.AddComponent<NewMeshComponent>();
        m_rebuild_packet = true;
    }

    void OnMeshRendererComponentDestroyed(entt::basic_registry<entt::entity>&, entt::entity entity)
    {
    }

    void OnCameraComponentCreated(entt::basic_registry<entt::entity>&, entt::entity entity)
    {
        Entity e(entity, m_scene);
        e.GetComponent<CameraComponent>().camera = CreateRef<yoyo::Camera>();

        e.AddComponent<NewCameraComponent>();
        m_rebuild_packet = true;
    }

    void OnCameraComponentDestroyed(entt::basic_registry<entt::entity>&, entt::entity entity)
    {
    }

    void OnDirectionalLightComponentCreated(entt::basic_registry<entt::entity>&, entt::entity entity)
    {
        Entity e(entity, m_scene);
        e.GetComponent<DirectionalLightComponent>().dir_light = CreateRef<yoyo::DirectionalLight>();

        e.AddComponent<NewDirectionalLightComponent>();
        m_rebuild_packet = true;
    }

    void OnDirectionalLightComponentDestroyed(entt::basic_registry<entt::entity>&, entt::entity entity)
    {
    }

    LayerType(GameLayer)
private:
    // Systems
    Ref<SceneGraph> m_scene_graph;
    Ref<psx::PhysicsWorld> m_physics_world;

    Scene* m_scene;

    yoyo::RenderPacket* m_rp;
    bool m_rebuild_packet = true;

    yoyo::RendererLayer* m_renderer_layer;
    yoyo::Application& m_app;
};

class Sandbox : public yoyo::Application
{
public:
    Sandbox()
        : yoyo::Application({ "Sandbox", 0, 0, 1920, 1080 })
    {
        PushLayer(Y_NEW GameLayer(*(yoyo::Application*)this));
    }

    ~Sandbox()
    {
    }
};

yoyo::Application* CreateApplication()
{
    return Y_NEW Sandbox;
};