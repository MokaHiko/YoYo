#pragma once

#include "Core/Memory.h"
#include "Core/Layer.h"

#include "Resource.h"

namespace yoyo{
    class Mesh;
    class MeshCreatedEvent;

    class Material;

    class Shader;

    class Texture;

    class ResourceManager
    {
    public:
        static void Init();
        static void Shutdown();

        template<typename T>
        static Ref<T> Load(const std::string& path);

        template<typename T>
        static void Free(Ref<T> resource);

        static void Update();
    public:
        // Callbacks
        static bool OnMeshCreated(Ref<Mesh> mesh);
        static bool OnShaderCreated(Ref<Shader> shader);
        static bool OnTextureCreated(Ref<Texture> texture);
        static bool OnMaterialCreated(Ref<Material> material);
    private:
        ResourceManager();
        ~ResourceManager();

        inline static std::unordered_map<ResourceId, Ref<Mesh>> m_mesh_cache;
        inline static std::unordered_map<ResourceId, Ref<Texture>> m_texture_cache;
        inline static std::unordered_map<ResourceId, Ref<Shader>> m_shader_cache;
        inline static std::unordered_map<ResourceId, Ref<Material>> m_material_cache;
    };

    class Event;

    // Manages the runtime resources of the application (meshes, textures, audio, etc...) via a static/singleton ResourceManager 
    class RuntimeResourceLayer : public Layer
    {
    public:
        RuntimeResourceLayer() = default;
        virtual ~RuntimeResourceLayer();

        virtual void OnAttach() override;
        virtual void OnDetatch() override;

        virtual void OnUpdate(float dt) override;

        virtual void OnEnable() override;
        virtual void OnDisable() override;
    };
}