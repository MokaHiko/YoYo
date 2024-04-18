#pragma once

#include "Core/Memory.h"
#include "Math/Math.h"

#include "RenderTypes.h"

namespace yoyo
{
    class IMesh;
    class Material;

    // The base class for all renderable objects in a scene
    class RenderableObject
    {
    public:
        RenderSceneId Id() const { return m_id; }
        void SetId(RenderSceneId id) { m_id = id; }

        // Returns whether or not a renderable object is valid within the scene
        const bool Valid() const {return m_id != NULL_RENDER_SCENE_ID;}

        // How many shader passes this object is a part of
        const uint32_t ShaderPassCount() const { return m_render_pass_count; }
    private:
        friend class MeshPass;
        void IncrementShaderPassCount() { m_render_pass_count++; }
        void DecrementShaderPassCount() { m_render_pass_count--; }

        uint32_t m_render_pass_count = 0;
        RenderSceneId m_id = NULL_RENDER_SCENE_ID;
    };

    // A renderable object that has a 3d location in a scene
    struct MeshPassObject : public RenderableObject
    {
        Ref<IMesh> mesh = nullptr;
        Ref<Material> material = nullptr;
        Vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
        Mat4x4 model_matrix = {};
    };

    const RenderableBatchId GenerateBatchId(Ref<IMesh>& mesh, const Ref<Material>& material);

    // A renderable batch is a grouping of renderable objects that share the same Mesh and Material.
    struct RenderableBatch
    {
        RenderableBatch(Ref<IMesh> mesh_, Ref<Material> material_)
            :material(material_), mesh(mesh_), id(GenerateBatchId(mesh, material)){}

        virtual ~RenderableBatch() = default;

        Ref<Material> material;
        Ref<IMesh> mesh;

        // Only used when material of batch is instanced as the starting index into the InstanceData buffer.
        uint32_t instance_index = 0;

        std::vector<Ref<MeshPassObject>> renderables;
        RenderableBatchId id = 0;
    };

    // TODO: Change name to shader pass
    // Ex. forward pass, shadow pass, 
    struct MeshPass
    {
    public:
        MeshPass(uint32_t max_objects);
        ~MeshPass() = default;

        void AddRenderable(Ref<MeshPassObject> obj);
        void RemoveRenderable(RenderSceneId id);

        // A vector of renderables this mesh pass uses
        std::vector<Ref<MeshPassObject>> renderables;

        // Mapping between renderable ids and indices 
        std::vector<int> renderable_id_to_index = {};

        // Flag vector of renderable ids
        std::vector<RenderSceneId> renderable_ids = {};

        // Amount of renderables the current mesh contains
        uint32_t count = 0;

        MeshPassType type;
        uint32_t renderpass_id;  // TODO: use as index in render passes array
    };
}