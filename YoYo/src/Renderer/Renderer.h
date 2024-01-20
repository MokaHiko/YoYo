#pragma once

#include "Defines.h"
#include "Core/Memory.h"

#include "RenderScene.h"

namespace yoyo
{
    enum class RendererType
    {
        Uknown = 0,
        Vulkan,
        Directx,
        Moltenvk,
    };

    struct YAPI RendererSettings
    {
        RendererType type;
        int max_frames_in_flight;
        bool tesselation;
    };

    // Structure that sends info about updates to the scene
    struct YAPI RenderPacket
    {
        double dt;
    };

    class YAPI Renderer
    {
    public:
        Renderer(const RendererSettings& settings) 
            :m_settings(settings) {}
        virtual ~Renderer() {};

        const RendererType Type() const {return m_settings.type;}
        const RendererSettings& Settings() const {return m_settings;}
    public:
        virtual void Init() {};
        virtual void Shutdown() {};

        virtual bool BeginFrame(const RenderPacket& rp) = 0;
        virtual void EndFrame() = 0;
    protected:
        RenderScene m_scene;
        RendererSettings m_settings;
    };

    extern Ref<Renderer> CreateRenderer();
};

 