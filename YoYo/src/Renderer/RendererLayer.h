#pragma once

#include "Core/Layer.h"
#include "Core/Memory.h"

#include "Renderer.h"
#include "Defines.h"

namespace yoyo
{
    class YAPI RendererLayer : public Layer
    {
    public:
        RendererLayer();
        virtual ~RendererLayer();

        void SendRenderPacket(RenderPacket* packet);

        virtual void OnAttach() override;
        virtual void OnDetatch() override;

        virtual void OnEnable() override;
        virtual void OnDisable() override;

        virtual void OnUpdate(float dt) override;

        LayerType(RendererLayer)
    private:
        Ref<RenderScene> m_scene;
        Ref<Renderer> m_renderer;
    };
}