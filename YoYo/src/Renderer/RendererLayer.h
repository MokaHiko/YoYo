#pragma once

#include "Core/Layer.h"
#include "Core/Memory.h"

#include "Renderer.h"
#include "Defines.h"

namespace yoyo
{
    class Application;
    class YAPI RendererLayer : public Layer
    {
    public:
        RendererLayer(Application* app);
        virtual ~RendererLayer();

        void SendRenderPacket(RenderPacket* packet);

        Ref<Renderer> GetRenderer() {return m_renderer;}
        void* NativeRenderer();

        virtual void OnAttach() override;
        virtual void OnDetatch() override;

        virtual void OnEnable() override;
        virtual void OnDisable() override;

        virtual void OnUpdate(float dt) override;

        virtual void OnImGuiRender() override;

        LayerType(RendererLayer)
    private:
        float m_dt;
        Ref<RenderScene> m_scene;
        Ref<Renderer> m_renderer;

        Application* m_app;
    };
}