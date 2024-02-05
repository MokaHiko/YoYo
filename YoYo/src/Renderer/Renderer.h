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

        float width;
        float height;
        float fov;
    };

    // Structure that sends info about updates to the scene
    struct YAPI RenderPacket
    {
        double dt;
    };

    const int MAX_OBJECTS = 1000;
    const int MAX_DIR_LIGHTS = 2;
    const int MAX_POINT_LIGHTS = 100;

    class YAPI Renderer
    {
    public:
        Renderer(const RendererSettings& settings) 
            :m_settings(settings) {}
        virtual ~Renderer() {};

        RenderScene& Scene() {return m_scene;}

        const RendererType Type() const {return m_settings.type;}
        const RendererSettings& Settings() const {return m_settings;}

        RendererSettings& Settings()
        {
            m_diry_flags = true;
            return m_settings;
        }
    public:
        virtual void Init() {};
        virtual void Shutdown() {};

        virtual bool BeginFrame(const RenderPacket& rp) = 0;
        virtual void EndFrame() = 0;
    private:
        RenderScene m_scene;
        RendererSettings m_settings;
        bool m_diry_flags;
    };

    extern Ref<Renderer> CreateRenderer();
};

 