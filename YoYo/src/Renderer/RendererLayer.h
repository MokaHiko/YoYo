#pragma once

#include "Core/Layer.h"
#include "Core/Memory.h"

#include "Renderer.h"
#include "Defines.h"

namespace yoyo
{
    YAPI class RendererLayer : public Layer
    {
    public:
        RendererLayer();
        virtual ~RendererLayer();

        virtual void OnAttach();
        virtual void OnDetatch();

        virtual void OnEnable();
        virtual void OnDisable();

        virtual void OnUpdate(float dt);
    private:
        Ref<Renderer> m_renderer;
    };
}