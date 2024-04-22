#pragma once

#include "Defines.h"
#include "Core/Memory.h"
#include "Math/MathTypes.h"

#include "RenderTypes.h"

namespace yoyo
{
    enum class RendererType
    {
        Uknown = 0,
        Vulkan,
        Directx,
        Moltenvk,
    };

    struct RendererProfile
    {
        uint32_t draw_calls = 0;
    };

    struct YAPI RendererSettings
    {
        RendererType type;
        int max_frames_in_flight;
        bool tesselation;

        uint32_t width;
        uint32_t height;
        float fov;
    };

    const uint32_t MAX_OBJECTS = 10000;
    const int MAX_DIR_LIGHTS = 2;
    const int MAX_POINT_LIGHTS = 100;
    
    class RenderScene;
    class Texture;
    class YAPI Renderer
    {
    public:
        Renderer(const RendererSettings& settings) 
            :m_settings(settings) {}
        virtual ~Renderer() {};

        const RendererType Type() const {return m_settings.type;}
        const RendererSettings& Settings() const {return m_settings;}

        RendererSettings& Settings()
        {
            m_diry_flags = true;
            return m_settings;
        }

        RendererProfile& Profile()
        {
            return m_profile;
        }
    public:
        virtual void Init() {};
        virtual void Shutdown() {};

        virtual const Ref<Texture>& GetViewPortTexture() const = 0; // The render context of the current frame 

        virtual void* RenderContext() = 0; // The render context of the current frame 

        virtual bool BeginFrame(Ref<RenderScene> scene) = 0;

        virtual void BeginBlitPass() = 0;

        virtual void EndBlitPass() = 0;

        virtual void EndFrame() = 0;

        // Returns the current frame being rendered
        virtual const uint32_t GetCurrentFrame() const = 0;
    protected:
        RendererProfile m_profile;
        RendererSettings m_settings;
        bool m_diry_flags;
    };

    extern Ref<Renderer> CreateRenderer();
};

 