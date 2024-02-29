#pragma once

#include "Core/Memory.h"
#include "Math/Math.h"

namespace yoyo
{
    // Unique identifier of a renderable object in a RenderScene.
    using RenderSceneId = uint32_t;
    const RenderSceneId NULL_RENDER_SCENE_ID = -1;

    // Unique identifier of a renderable batch in a RenderScene.
    using RenderableBatchId = uint64_t;

    class Mesh;
    class Material;

    class Camera;
    class DirectionalLight;
    class PointLight;

    class RenderableObject
    {
    public:
        RenderSceneId Id() const { return m_id; }
    private:
        friend class RenderScene;
        RenderSceneId m_id = NULL_RENDER_SCENE_ID;
    };

    class RenderPass
    {
    public:
        RenderPass() = default;
        virtual ~RenderPass() = default;

    private:

    };

    enum class MeshPassType
    {
        Forward,
        ForwardTransparent,
        Shadow,
        PostProcess,
    };

    struct MeshPassObject
    {
        Ref<Mesh> mesh;
        Ref<Material> material;
        Mat4x4 model_matrix;

        RenderSceneId Id() const { return m_id; }
    private:
        friend class RenderScene;
        RenderSceneId m_id = NULL_RENDER_SCENE_ID;
    };

	const RenderableBatchId GenerateBatchId(const Ref<Mesh>& mesh, const Ref<Material>& material);

    // A renderable batch is a grouping of renderable objects that share the same Mesh and Material.
    class RenderableBatch
    {
    public:
        RenderableBatch(Ref<Mesh> mesh, Ref<Material> material);
        virtual ~RenderableBatch() = default;

        Ref<Material> material;
        Ref<Mesh> mesh;

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