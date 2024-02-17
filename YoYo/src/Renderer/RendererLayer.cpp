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

	void RendererLayer::SendRenderPacket(RenderPacket* packet)
	{
        if(packet->new_camera)
        {
            m_scene->camera = packet->new_camera;
        }

        for(auto id : packet->deleted_dir_lights)
        {
        }

        for(auto& light: packet->new_dir_lights)
        {
            m_scene->directional_lights.push_back(light);
        }

        // Remove deleted objects
        for(auto id : packet->deleted_objects)
        {

        }

        // TODO: Sort by material and if use shadows
        // Add new objects
        for(auto& obj : packet->new_objects)
        {
            m_scene->forward_pass->renderables.push_back(obj);

            if(obj->material->receive_shadows)
            {
                m_scene->shadow_pass->renderables.push_back(obj);
            }
        }

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

        // Renderer settings
        m_renderer->Settings().width = 720;
        m_renderer->Settings().height = 480;

        YINFO("Renderer Type: %s", renderer_type_strings[(int)m_renderer->Type()]);
    }

    void RendererLayer::OnDetatch()
    {
        m_renderer->Shutdown();
    }

    void RendererLayer::OnEnable()
    {
        m_renderer->Init();
        m_scene = CreateRef<RenderScene>();
    }

    void RendererLayer::OnDisable()
    {
    }

    void RendererLayer::OnUpdate(float dt)
    {
        m_renderer->BeginFrame(m_scene);

        m_renderer->EndFrame();
    }
}
