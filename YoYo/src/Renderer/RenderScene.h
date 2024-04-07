#pragma once

#include "Core/Memory.h"
#include "Core/Assert.h"

#include "Renderer/RenderPass.h"
namespace yoyo
{
    class Camera;
    class DirectionalLight;
    class PointLight;

    // Render structure that is incrementally built and updated.
    class RenderScene
    {
    public:
        RenderScene(uint32_t max_renderable_objects);
        virtual ~RenderScene();

        // Adds a mesh pass object to the scene.
        void AddMeshPassObject(Ref<MeshPassObject> obj);

        // Removes a mesh pass object from the scene
        void RemoveMeshPassObject(RenderSceneId obj_id);

        // Groups RenderableObjects by mesh & material.
        void BuildFlatBatches();

        // Flat batches are RenderableObjects grouped by mesh & material.
        std::vector<Ref<RenderableBatch>> forward_flat_batches;

        const Ref<MeshPass>& GetShadowPass() const { return shadow_pass; }
        uint32_t GetShadowPassCount() const { return shadow_pass_count; }

        const Ref<MeshPass>& GetForwardPass() const { return forward_pass; }
        uint32_t GetForwardPassCount() const { return forward_pass_count; }
    public:
        std::vector<Ref<DirectionalLight>> directional_lights;
        std::vector<Ref<PointLight>> point_lights;

        Ref<Camera> camera;
   private:
        Ref<MeshPass> shadow_pass = nullptr;
        uint32_t shadow_pass_count = 0;

        Ref<MeshPass> forward_pass = nullptr;
        uint32_t forward_pass_count = 0;

        Ref<MeshPass> transparent_forward_pass = nullptr;
        uint32_t transparent_forward_pass_count = 0;
    private:
        virtual RenderSceneId GenerateSceneId();
        std::set<RenderSceneId> m_free_ids = {};
        RenderSceneId m_next_id = 0;
    };
}