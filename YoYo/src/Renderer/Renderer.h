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

        float width;
        float height;
        float fov;
    };

    // Structure that sends info about updates to the scene
    class Camera;
    class DirectionalLight;

    class MeshPassObject;
    struct YAPI RenderPacket
    {
        Mat4x4 view;
        Mat4x4 proj;

        void Clear()
        {
            new_dir_lights.clear();
            deleted_dir_lights.clear();

            new_objects.clear();
            deleted_objects.clear();

            new_camera.reset();
            new_camera = nullptr;
        }

        std::vector<uint32_t> deleted_dir_lights;
        std::vector<Ref<DirectionalLight>> new_dir_lights;

        std::vector<uint32_t> deleted_objects;
        std::vector<Ref<MeshPassObject>> new_objects;

        Ref<Camera> new_camera;

        double dt;
    };

    const uint32_t MAX_OBJECTS = 1000;
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
    private:
        RendererProfile m_profile;
        RendererSettings m_settings;
        bool m_diry_flags;
    };

    extern Ref<Renderer> CreateRenderer();
};

 