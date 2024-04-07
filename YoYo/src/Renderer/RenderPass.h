#pragma once

#include "Core/Memory.h"
#include "Math/Math.h"

#include "RenderTypes.h"

namespace yoyo
{
    class IMesh;
    class Material;

    class RenderableObject
    {
    public:
        RenderSceneId Id() const { return m_id; }
    private:
        friend class RenderScene;
        RenderSceneId m_id = NULL_RENDER_SCENE_ID;
    };

    struct MeshPassObject
    {
        Ref<IMesh> mesh;
        Ref<Material> material;
        Mat4x4 model_matrix;

        RenderSceneId Id() const { return m_id; }
    private:
        friend class RenderScene;
        RenderSceneId m_id = NULL_RENDER_SCENE_ID;
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

    // Ex. forward pass, shadow pass, 
    struct MeshPass
    {
    public:
        std::vector<Ref<MeshPassObject>> renderables;

        MeshPassType type;
        uint32_t renderpass_id;  // TODO: use as index in render passes array
    };
}