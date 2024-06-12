#pragma once

#include "Defines.h"
#include "Core/Memory.h"
#include "Math/MathTypes.h"

#include "RenderTypes.h"

namespace yoyo
{
	enum class RendererDirtyFlags
	{
		Clean = 0,
		WindowResize = 1,
		FormatChange= 1 << 1,
	};

	inline RendererDirtyFlags operator~ (RendererDirtyFlags a) { return (RendererDirtyFlags)~(int)a; }
	inline RendererDirtyFlags operator| (RendererDirtyFlags a, RendererDirtyFlags b) { return (RendererDirtyFlags)((int)a | (int)b); }
	inline RendererDirtyFlags operator& (RendererDirtyFlags a, RendererDirtyFlags b) { return (RendererDirtyFlags)((int)a & (int)b); }
	inline RendererDirtyFlags operator^ (RendererDirtyFlags a, RendererDirtyFlags b) { return (RendererDirtyFlags)((int)a ^ (int)b); }
	inline RendererDirtyFlags& operator|= (RendererDirtyFlags& a, RendererDirtyFlags b) { return (RendererDirtyFlags&)((int&)a |= (int)b); }
	inline RendererDirtyFlags& operator&= (RendererDirtyFlags& a, RendererDirtyFlags b) { return (RendererDirtyFlags&)((int&)a &= (int)b); }
	inline RendererDirtyFlags& operator^= (RendererDirtyFlags& a, RendererDirtyFlags b) { return (RendererDirtyFlags&)((int&)a ^= (int)b); }

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

    const uint32_t MAX_OBJECTS = 100000;
    const int MAX_DIR_LIGHTS = 2;
    const int MAX_POINT_LIGHTS = 100;
    
    class RenderScene;
    class Texture;
    class YAPI Renderer
    {
    public:
        Renderer(const RendererSettings& settings) 
            :m_settings(settings), m_diry_flags(RendererDirtyFlags::Clean) {}
        virtual ~Renderer() {};

        const RendererType Type() const {return m_settings.type;}
        const RendererSettings& Settings() const {return m_settings;}

        RendererSettings& Settings()
        {
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
        RendererDirtyFlags m_diry_flags;
    };

    extern Ref<Renderer> CreateRenderer();
};