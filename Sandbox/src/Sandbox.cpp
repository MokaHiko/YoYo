#include "Sandbox.h"

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

#include "Scripts/CameraController.h"
#include "Scripts/Sun.h"
#include "Scripts/VillageManager.h"

#include "SceneGraph/SceneGraph.h"
#include "Physics/Physics3D.h"

#include "Editor/EditorLayer.h"

GameLayer::GameLayer(yoyo::Application& app)
    :m_app(app) {}

GameLayer::~GameLayer() {}

void GameLayer::OnAttach()
{
    // Init scene
    m_scene = Y_NEW Scene();

    // Init systems
    m_scene_graph = CreateRef<SceneGraph>(m_scene);
    m_physics_world = CreateRef<psx::PhysicsWorld>(m_scene);
    m_scripting = CreateRef<ScriptingSystem>(m_scene);

    // Init render packet
    m_rp = Y_NEW yoyo::RenderPacket;
}

void GameLayer::OnDetatch()
{
    // Clean up handles
    delete m_rp;
    delete m_scene;
}

void GameLayer::OnEnable()
{
    m_renderer_layer = m_app.FindLayer<yoyo::RendererLayer>();

    m_scene->Registry().on_construct<CameraComponent>().connect<&GameLayer::OnCameraComponentCreated>(this);
    m_scene->Registry().on_destroy<CameraComponent>().connect<&GameLayer::OnCameraComponentDestroyed>(this);

    m_scene->Registry().on_construct<DirectionalLightComponent>().connect<&GameLayer::OnDirectionalLightComponentCreated>(this);
    m_scene->Registry().on_destroy<DirectionalLightComponent>().connect<&GameLayer::OnDirectionalLightComponentDestroyed>(this);

    m_scene->Registry().on_construct<MeshRendererComponent>().connect<&GameLayer::OnMeshRendererComponentCreated>(this);
    m_scene->Registry().on_destroy<MeshRendererComponent>().connect<&GameLayer::OnMeshRendererComponentDestroyed>(this);

    m_scene_graph->Init();
    m_physics_world->Init();
    m_scripting->Init();

    // Load assets
    Ref<yoyo::Shader> default_lit = yoyo::ResourceManager::Instance().Load<yoyo::Shader>("lit_shader");
    Ref<yoyo::Shader> default_lit_instanced = yoyo::ResourceManager::Instance().Load<yoyo::Shader>("lit_instanced_shader");

    Ref<yoyo::Material> default_material = yoyo::Material::Create(default_lit, "default_material");
    Ref<yoyo::Texture> default_texture = yoyo::ResourceManager::Instance().Load<yoyo::Texture>("assets/textures/prototype_512x512_white.yo");
    default_material->SetTexture(yoyo::MaterialTextureType::MainTexture, default_texture);
    default_material->SetVec4("diffuse_color", yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
    default_material->SetVec4("specular_color", yoyo::Vec4{ 0.25, 0.0f, 0.0f, 1.0f });
    default_material->color = yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f };

    Ref<yoyo::Material> default_instanced_material = yoyo::Material::Create(default_lit_instanced, "default_instanced_material");
    Ref<yoyo::Texture> default_instanced_texture = yoyo::ResourceManager::Instance().Load<yoyo::Texture>("assets/textures/prototype_512x512_white.yo");
    default_instanced_material->SetTexture(yoyo::MaterialTextureType::MainTexture, default_instanced_texture);
    default_instanced_material->SetVec4("diffuse_color", yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
    default_instanced_material->SetVec4("specular_color", yoyo::Vec4{ 0.25, 0.0f, 0.0f, 1.0f });
    default_instanced_material->color = yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f };

    yoyo::ResourceManager::Instance().Load<yoyo::Model>("assets/models/plane.yo");
    Ref<yoyo::Model> cube_model = yoyo::ResourceManager::Instance().Load<yoyo::Model>("assets/models/cube.yo");
    std::reverse(cube_model->meshes[0]->vertices.begin(), cube_model->meshes[0]->vertices.end());

    // Claptrap model
    {
        auto claptrap_model = yoyo::ResourceManager::Instance().Load<yoyo::Model>("assets/models/claptrap.yo");
        auto claptrap_material = yoyo::Material::Create(default_lit_instanced, "claptrap_material");

        claptrap_material->SetTexture(yoyo::MaterialTextureType::MainTexture, yoyo::ResourceManager::Instance().Load<yoyo::Texture>("assets/textures/Claptrap_blender_fix_Claptrap_BaseColor.yo"));
        claptrap_material->SetTexture(yoyo::MaterialTextureType::SpecularMap, yoyo::ResourceManager::Instance().Load<yoyo::Texture>("assets/textures/Claptrap_blender_fix_Claptrap_Roughness.yo"));
        claptrap_material->color = yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
        claptrap_material->SetVec4("diffuse_color", yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
    }

    // Crate
    {
        Ref<yoyo::Material> crate_material = yoyo::Material::Create(default_lit_instanced, "crate_material");
        crate_material->SetTexture(yoyo::MaterialTextureType::MainTexture, yoyo::ResourceManager::Instance().Load<yoyo::Texture>("assets/textures/container2.yo"));
        crate_material->color = yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
        crate_material->SetVec4("diffuse_color", yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
        crate_material->SetVec4("specular_color", yoyo::Vec4{ 0.25, 0.0f, 0.0f, 1.0f });
    }

    // Universal game material
    Ref<yoyo::Material> colormap_material = yoyo::Material::Create(default_lit_instanced, "colormap_material");
    colormap_material->SetTexture(yoyo::MaterialTextureType::MainTexture, yoyo::ResourceManager::Instance().Load<yoyo::Texture>("assets/textures/colormap.yo"));
    colormap_material->color = yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    colormap_material->SetVec4("diffuse_color", yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
    colormap_material->SetVec4("specular_color", yoyo::Vec4{ 0.25, 0.0f, 0.0f, 1.0f });

    // Trunk
    {
        auto villager_model = yoyo::ResourceManager::Instance().Load<yoyo::Model>("assets/models/trunk.yo");
    }

    // Villager model
    {
        auto villager_model = yoyo::ResourceManager::Instance().Load<yoyo::Model>("assets/models/villager.yo");
    }

    // Lights
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

    // Set up scene
    auto camera = m_scene->Instantiate("camera", { 0.0f, 5.0f, 20.0f });
    camera.AddComponent<CameraComponent>().camera->SetType(yoyo::CameraType::Orthographic);
    camera.AddComponent<CameraControllerComponent>(camera);

    // Village Manager
    {
        auto village_manager = m_scene->Instantiate("village_manager", { 0.0f, 0.0f, 0.0f });

        auto& village_props = village_manager.AddComponent<VillageProps>();
        village_props.max_villagers = 1;
        village_props.spawn_rate = 1.0f;

        village_manager.AddComponent<VillageManagerComponent>(village_manager);
    }

    // Plane
    {
        Ref<yoyo::Material> grid_material = yoyo::Material::Create(default_lit_instanced, "grid_material");
        Ref<yoyo::Texture> grid_texture = yoyo::ResourceManager::Instance().Load<yoyo::Texture>("assets/textures/prototype_512x512_white.yo");
        grid_texture->SetSamplerType(yoyo::TextureSamplerType::Linear);

        grid_material->SetTexture(yoyo::MaterialTextureType::MainTexture, grid_texture);
        grid_material->color = yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
        grid_material->SetVec4("diffuse_color", yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
        grid_material->SetVec4("specular_color", yoyo::Vec4{ 0.25, 0.0f, 0.0f, 1.0f });

        Entity floors = m_scene->Instantiate("floors");
        TransformComponent& floor_transform = floors.GetComponent<TransformComponent>();

        float root = 5;
        float dim = 16.0f;
        for (int j = -root; j < root; j++)
        {
            for (int i = -root; i < root; i++)
            {
                auto plane = m_scene->Instantiate("plane", { dim * 2.0f * i, 0.0f, dim * 2.0f * j });

                TransformComponent& transform = plane.GetComponent<TransformComponent>();
                transform.scale = { dim, dim, 1.0f };
                transform.quat_rotation = yoyo::QuatFromAxisAngle({ 1, 0, 0 }, yoyo::DegToRad(-90));

                MeshRendererComponent& mesh_renderer = plane.AddComponent<MeshRendererComponent>();
                mesh_renderer.mesh = yoyo::ResourceManager::Instance().Load<yoyo::Mesh>("planePlane0");
                mesh_renderer.material = grid_material;

                floor_transform.AddChild(plane);
            }
        }
    }
}

void GameLayer::OnDisable()
{
    m_renderer_layer = nullptr;

    m_scene->Registry().on_construct<CameraComponent>().disconnect<&GameLayer::OnCameraComponentCreated>(this);
    m_scene->Registry().on_destroy<CameraComponent>().disconnect<&GameLayer::OnCameraComponentDestroyed>(this);

    m_scene->Registry().on_construct<DirectionalLightComponent>().disconnect<&GameLayer::OnDirectionalLightComponentCreated>(this);
    m_scene->Registry().on_destroy<DirectionalLightComponent>().disconnect<&GameLayer::OnDirectionalLightComponentDestroyed>(this);

    m_scene->Registry().on_construct<MeshRendererComponent>().disconnect<&GameLayer::OnMeshRendererComponentCreated>(this);
    m_scene->Registry().on_destroy<MeshRendererComponent>().disconnect<&GameLayer::OnMeshRendererComponentDestroyed>(this);

    // Shutdown Systems
    m_scripting->Shutdown();
    m_physics_world->Shutdown();
    m_scene_graph->Shutdown();
};

void GameLayer::OnUpdate(float dt)
{
    // Physics System
    m_physics_world->Update(dt);

    // Scene Graph
    m_scene_graph->Update(dt);

    // Update Render Scene Mesh
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
        m_scripting->Update(dt);
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

            bool res = e.RemoveComponent<NewCameraComponent>();
            YASSERT(res, "Failed to remove new mesh component!");
            break;
        }

        // Lights
        for (auto& id : m_scene->Registry().view<NewDirectionalLightComponent, DirectionalLightComponent>())
        {
            Entity e{ id, m_scene };

            TransformComponent& transform = e.GetComponent<TransformComponent>();
            Ref<yoyo::DirectionalLight> dir_light = e.GetComponent<DirectionalLightComponent>().dir_light;

            m_rp->new_dir_lights.emplace_back(dir_light);

            bool res = e.RemoveComponent<NewDirectionalLightComponent>();
            YASSERT(res, "Failed to remove new mesh component!");
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

            bool res = e.RemoveComponent<NewMeshComponent>();
            YASSERT(res, "Failed to remove new mesh component!");
        }

        m_rebuild_packet = false;
    }
    m_renderer_layer->SendRenderPacket(m_rp);
};

void GameLayer::OnMeshRendererComponentCreated(entt::basic_registry<entt::entity>&, entt::entity entity)
{
    Entity e(entity, m_scene);
    e.GetComponent<MeshRendererComponent>().mesh_object = CreateRef<yoyo::MeshPassObject>();
    e.AddComponent<NewMeshComponent>();

    m_rebuild_packet = true;
}

void GameLayer::OnMeshRendererComponentDestroyed(entt::basic_registry<entt::entity>&, entt::entity entity)
{
}

void GameLayer::OnCameraComponentCreated(entt::basic_registry<entt::entity>&, entt::entity entity)
{
    Entity e(entity, m_scene);
    e.GetComponent<CameraComponent>().camera = CreateRef<yoyo::Camera>();

    e.AddComponent<NewCameraComponent>();
    m_rebuild_packet = true;
}

void GameLayer::OnCameraComponentDestroyed(entt::basic_registry<entt::entity>&, entt::entity entity)
{
}

void GameLayer::OnDirectionalLightComponentCreated(entt::basic_registry<entt::entity>&, entt::entity entity)
{
    Entity e(entity, m_scene);
    e.GetComponent<DirectionalLightComponent>().dir_light = CreateRef<yoyo::DirectionalLight>();

    e.AddComponent<NewDirectionalLightComponent>();
    m_rebuild_packet = true;
}

void GameLayer::OnDirectionalLightComponentDestroyed(entt::basic_registry<entt::entity>&, entt::entity entity)
{
}

class Sandbox : public yoyo::Application
{
public:
    Sandbox()
        : yoyo::Application({ "Sandbox", 0, 0, 1920, 1080 })
    {
        PushLayer(Y_NEW EditorLayer(this));
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