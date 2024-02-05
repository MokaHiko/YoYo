#include "RendererLayer.h"
#include "Core/Log.h"

// TODO: REMOVE 
#include "Resource/ResourceManager.h"
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"

#include "Math/MatrixTransform.h"

namespace yoyo
{
    RendererLayer::RendererLayer()
        : m_renderer(nullptr)
    {
    }

    RendererLayer::~RendererLayer()
    {
    }

    void RendererLayer::OnAttach()
    {
        static const char* renderer_type_strings[]
        {
            "Uknown",
            "Vulkan",
            "DirectX",
            "MoltenVK"
        };

        m_renderer = CreateRenderer();
        YINFO("Renderer Type: %s", renderer_type_strings[(int)m_renderer->Type()]);

        m_renderer->Init();
    }

    void RendererLayer::OnDetatch()
    {
        m_renderer->Shutdown();
    }

    void RendererLayer::OnEnable()
    {
        auto cube = Mesh::Create("cube");
        cube->vertices = {
        {{ 1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  1.00,  0.00},  {0.625,  0.50}},
        {{-1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  1.00,  0.00},  {0.875,  0.50}},
        {{ 1.00,  1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  1.00,  0.00},  {0.625,  0.25}},
        {{-1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  1.00,  0.00},  {0.875,  0.50}},
        {{-1.00,  1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  1.00,  0.00},  {0.875,  0.25}},
        {{ 1.00,  1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  1.00,  0.00},  {0.625,  0.25}},
        {{ 1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00,  1.00},  {0.375,  0.25}},
        {{ 1.00,  1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00,  1.00},  {0.625,  0.25}},
        {{-1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00,  1.00},  {0.375,  0.00}},
        {{ 1.00,  1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00,  1.00},  {0.625,  0.25}},
        {{-1.00,  1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00,  1.00},  {0.625,  0.00}},
        {{-1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00,  1.00},  {0.375,  0.00}},
        {{-1.00, -1.00,  1.00}, {1.00, 1.00, 1.00}, {-1.00,  0.00,  0.00},  {0.375,  1.00}},
        {{-1.00,  1.00,  1.00}, {1.00, 1.00, 1.00}, {-1.00,  0.00,  0.00},  {0.625,  1.00}},
        {{-1.00, -1.00, -1.00}, {1.00, 1.00, 1.00}, {-1.00,  0.00,  0.00},  {0.375,  0.75}},
        {{-1.00,  1.00,  1.00}, {1.00, 1.00, 1.00}, {-1.00,  0.00,  0.00},  {0.625,  1.00}},
        {{-1.00,  1.00, -1.00}, {1.00, 1.00, 1.00}, {-1.00,  0.00,  0.00},  {0.625,  0.75}},
        {{-1.00, -1.00, -1.00}, {1.00, 1.00, 1.00}, {-1.00,  0.00,  0.00},  {0.375,  0.75}},
        {{-1.00, -1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00, -1.00,  0.00},  {0.125,  0.50}},
        {{ 1.00, -1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00, -1.00,  0.00},  {0.375,  0.50}},
        {{-1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00, -1.00,  0.00},  {0.125,  0.25}},
        {{ 1.00, -1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00, -1.00,  0.00},  {0.375,  0.50}},
        {{ 1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00, -1.00,  0.00},  {0.375,  0.25}},
        {{-1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {0.00, -1.00,  0.00},  {0.125,  0.25}},
        {{ 1.00, -1.00, -1.00}, {1.00, 1.00, 1.00},  {1.00,  0.00,  0.00},  {0.375,  0.50}},
        {{ 1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {1.00,  0.00,  0.00},  {0.625,  0.50}},
        {{ 1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {1.00,  0.00,  0.00},  {0.375,  0.25}},
        {{ 1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {1.00,  0.00,  0.00},  {0.625,  0.50}},
        {{ 1.00,  1.00,  1.00}, {1.00, 1.00, 1.00},  {1.00,  0.00,  0.00},  {0.625,  0.25}},
        {{ 1.00, -1.00,  1.00}, {1.00, 1.00, 1.00},  {1.00,  0.00,  0.00},  {0.375,  0.25}},
        {{-1.00, -1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00, -1.00},  {0.375,  0.75}},
        {{-1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00, -1.00},  {0.625,  0.75}},
        {{ 1.00, -1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00, -1.00},  {0.375,  0.50}},
        {{-1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00, -1.00},  {0.625,  0.75}},
        {{ 1.00,  1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00, -1.00},  {0.625,  0.50}},
        {{ 1.00, -1.00, -1.00}, {1.00, 1.00, 1.00},  {0.00,  0.00, -1.00},  {0.375,  0.50}},
        };

        // TODO: Move to application layer
        Ref<Camera> main_camera = CreateRef<Camera>();
        m_renderer->Scene().main_camera = main_camera;

        Ref<DirectionalLight> m_dir_light = CreateRef<DirectionalLight>();
        m_dir_light->color = Vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
        m_dir_light->direction = Vec4{ 1.0f, 1.0f, 1.0f, 1.0f } *-1.0f;
        m_renderer->Scene().directional_lights.push_back(m_dir_light);

        Ref<Material> crate_material = Material::Create(ResourceManager::Load<Shader>("lit_shader"));
        crate_material->SetTexture(MaterialTextureType::MainTexture, ResourceManager::Load<Texture>("assets/textures/container2.yo"));
        crate_material->color = Vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
        crate_material->SetVec4("diffuse_color", Vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
        crate_material->SetVec4("specular_color", Vec4{ 0.25, 0.0f, 0.0f, 1.0f });

        Ref<Material> red_material = Material::Create(ResourceManager::Load<Shader>("lit_shader"));
        red_material->SetTexture(MaterialTextureType::MainTexture, ResourceManager::Load<Texture>("assets/textures/container2.yo"));
        red_material->color = Vec4{ 1.0f, 0.0f, 0.0f, 1.0f };
        red_material->SetVec4("diffuse_color", Vec4{ 1.0f, 0.0f, 1.0f, 1.0f });
        red_material->SetVec4("specular_color", Vec4{ 0.25, 0.0f, 0.0f, 1.0f });

        // TODO: Make scene incremental build
        {
            Ref<MeshPassObject> pass_obj = CreateRef<MeshPassObject>();
            pass_obj->mesh = cube;
            pass_obj->material = crate_material;
            pass_obj->model_matrix = TranslationMat4x4(Vec3{-2, 0, -5.0f});
            pass_obj->id = 0;
            m_renderer->Scene().forward_pass->renderables.push_back(pass_obj);
        }
        {
            Ref<MeshPassObject> pass_obj = CreateRef<MeshPassObject>();
            pass_obj->mesh = cube;
            pass_obj->material = red_material;
            pass_obj->model_matrix = TranslationMat4x4(Vec3{2, 0, -5.0f});
            pass_obj->id = 1;
            m_renderer->Scene().forward_pass->renderables.push_back(pass_obj);
        }

        // {
        //     static float angle = 0;
        //     angle += (float)Time::DeltaTime();
        //     obj_data.model_matrix = RotateEulerMat4x4(Vec3{1.0f, 1.0f, 1.0f} * angle) * TranslationMat4x4({0, 0, -8.0f});
        // }
    }

    void RendererLayer::OnDisable()
    {
    }

    void RendererLayer::OnUpdate(float dt)
    {
        // TODO: Build Batches/Renderpacket that update scene
        RenderPacket rp = {};
        rp.dt = dt;

        m_renderer->BeginFrame(rp);

        m_renderer->EndFrame();
    }
}
