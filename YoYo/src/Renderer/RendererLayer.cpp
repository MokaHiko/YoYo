#include "RendererLayer.h"
#include "Core/Log.h"

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
    }

    void RendererLayer::OnDisable()
    {
    }

    void RendererLayer::OnUpdate(float dt)
    {
        // TODO: Build Batches/Renderpacket 

        RenderPacket rp;
        m_renderer->BeginFrame(rp);
        m_renderer->EndFrame();
    }
}
