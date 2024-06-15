#include "RendererLayer.h"
#include "Core/Log.h"

#include "Math/MatrixTransform.h"

#include "Core/Application.h"
#include "ImGui/ImGuiLayer.h"

#include "Core/Assert.h"

namespace yoyo
{
    RendererLayer::RendererLayer(Application* app)
        : m_renderer(nullptr), m_dt(0)
    {
        m_renderer = CreateRenderer();

        // Renderer settings
        m_renderer->Settings().width = app->Settings().width;
        m_renderer->Settings().height = app->Settings().height;

        m_app = app;
    }

    RendererLayer::~RendererLayer() {}

    void RendererLayer::SendRenderPacket(RenderPacket* packet)
    {
        // TODO: add scoped mutex
        m_render_packet_queue.emplace(packet);
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

        YINFO("Renderer Type: %s", renderer_type_strings[(int)m_renderer->Type()]);
    }

    void RendererLayer::OnDetatch()
    {
        m_renderer->Shutdown();
    }

    void RendererLayer::OnEnable()
    {
        m_renderer->Init();
        m_scene = CreateRef<RenderScene>(MAX_OBJECTS);
    }

    void RendererLayer::OnDisable()
    {
    }

    void RendererLayer::OnUpdate(float dt)
    {
        static ImGuiLayer* imgui_layer = m_app->FindLayer<ImGuiLayer>();
        static bool open = true;

        m_dt = dt;

        // Process render packets
        {
            if (m_render_packet_queue.size() > 1) 
            {
                //YINFO("RENDER PACKETS IN FLIGHT: %d", m_render_packet_queue.size());
            }

            bool build_batches = false;
            while (!m_render_packet_queue.empty())
            {
                RenderPacket* packet = m_render_packet_queue.front();

                if (!packet)
                {
                    YWARN("Null Render Packet was sent!");
                    m_render_packet_queue.pop();
                    continue;
                }

                if (packet->IsProccessed())
                {
                    YWARN("Sent packet that has already been processed!");
                    m_render_packet_queue.pop();
                    continue;
                }

                if (packet->new_camera)
                {
                    m_scene->camera = packet->new_camera;
                }

                for (auto id : packet->deleted_dir_lights)
                {
                }

                for (auto& light : packet->new_dir_lights)
                {
                    // TODO: Change to AddDirLight(); That assigns ID
                    m_scene->directional_lights.push_back(light);
                }

                // Add new objects
                for (auto& obj : packet->new_objects)
                {
                    m_scene->AddMeshPassObject(obj);
                    build_batches = true;
                }

                // Remove deleted objects
                for (auto& obj : packet->deleted_objects)
                {
                    m_scene->RemoveMeshPassObject(obj);
                    build_batches = true;
                }

                // Set process flag and remove from queue
                packet->SetProcessed(true);
                m_render_packet_queue.pop();
            }

            if (build_batches)
            {
                m_scene->BuildFlatBatches();
                build_batches = false;
            }
        }

        m_renderer->BeginFrame(m_scene);
        void* ctx = m_renderer->RenderContext();

        m_renderer->BeginBlitPass();
        imgui_layer->OnMainPassBegin(ctx);

        for (auto rit = m_app->Layers().rbegin(); rit != m_app->Layers().rend(); rit++)
        {
            (*rit)->OnImGuiRender();
        }

        imgui_layer->OnMainPassEnd(ctx);
        m_renderer->EndBlitPass();

        m_renderer->EndFrame();
    }

    void RendererLayer::OnImGuiRender()
    {
        ImGui::Begin("Renderer");

        ImGui::Text("%.3ffps / %.6fms", floor(1.0f/m_dt), m_dt);
        ImGui::Text("Shadow renderables: %d", m_scene->GetShadowPassCount());
        ImGui::Text("Forward renderables: %d", m_scene->GetForwardPassCount());
        ImGui::Text("Blit pass renderables: %d", 1);

        ImGui::Text("Draw calls: %d", m_renderer->Profile().draw_calls);

        ImGui::End();
    }
}
