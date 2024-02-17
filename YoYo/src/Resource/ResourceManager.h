#pragma once

#include "Defines.h"
#include "Core/Memory.h"
#include "Core/Layer.h"

#include "Resource.h"

namespace yoyo {
    class Mesh;
    class MeshCreatedEvent;

    class Material;

    class Shader;

    class Texture;

    class YAPI ResourceManager
    {
    public:
        static ResourceManager& Instance();

        void Init();
        void Shutdown();

        template<typename T>
        Ref<T> Load(const std::string& path);

        template<typename T>
        void Free(Ref<T> resource);

        void Update();
    public:
        // Callbacks
        bool OnMeshCreated(Ref<Mesh> mesh);
        bool OnShaderCreated(Ref<Shader> shader);
        bool OnTextureCreated(Ref<Texture> texture);
        bool OnMaterialCreated(Ref<Material> material);
    private:
        ResourceManager();
        ~ResourceManager();

        std::unordered_map<ResourceId, Ref<Mesh>> m_mesh_cache;
        std::unordered_map<ResourceId, Ref<Texture>> m_texture_cache;
        std::unordered_map<ResourceId, Ref<Shader>> m_shader_cache;
        std::unordered_map<ResourceId, Ref<Material>> m_material_cache;
    };

    class Event;

    // Manages the runtime resources of the application (meshes, textures, audio, etc...) via a static/singleton ResourceManager 
    class YAPI RuntimeResourceLayer : public Layer
    {
    public:
        RuntimeResourceLayer() = default;
        virtual ~RuntimeResourceLayer();

        virtual void OnAttach() override;
        virtual void OnDetatch() override;

        virtual void OnUpdate(float dt) override;

        virtual void OnEnable() override;
        virtual void OnDisable() override;

        LayerType(RuntimeResourceLayer)
    };
}