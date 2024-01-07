#pragma once

#include "Defines.h"
#include "Core/Memory.h"

namespace yoyo
{
    YAPI enum class RendererType
    {
        UKNOWN = 0,
        VULKAN,
        DIRECTX,
        MOLTENVK,
    };

    YAPI struct RendererSettings
    {
        RendererType type;
        int max_frames_in_flight;
        bool tesselation;
    };

    class MeshPass
    {
        std::vector<uint32_t> object_ids;
    };

    YAPI struct RenderPacket
    {
        double dt;
    };

    YAPI class Renderer
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
        RendererSettings m_settings;
    };

    extern Ref<Renderer> CreateRenderer();
};

 