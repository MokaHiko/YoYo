#pragma once

#include "Core/Memory.h"
#include "Core/Assert.h"

#include "Renderer/RenderPass.h"
namespace yoyo
{
    class Camera;
    class DirectionalLight;
    class PointLight;

    // Structure that sends info about updates to the scene
    // class Camera;
    // class DirectionalLight;
    class MeshPassObject;
    struct YAPI RenderPacket
    {
        void Reset()
        {
            new_dir_lights.clear();
            deleted_dir_lights.clear();

            new_objects.clear();
            deleted_objects.clear();

            new_camera.reset();
            new_camera = nullptr;

            m_processed = false;
        }

        std::vector<uint32_t> deleted_dir_lights;
        std::vector<Ref<DirectionalLight>> new_dir_lights;

        std::vector<Ref<MeshPassObject>> deleted_objects;
        std::vector<Ref<MeshPassObject>> new_objects;

        Ref<Camera> new_camera = nullptr;
        double time_stamp = 0.0f;

        // Boolean that indicates whether render packet was processed succesefully
        const bool IsProccessed() const
        {
            // TODO: Make thread safe
            return m_processed;
        }
    private:
        friend class RendererLayer;
        void SetProcessed(bool is_processed)
        {
            // TODO: Make thread safe
            m_processed = is_processed;
        }
    private:
        bool m_processed = false;
    };

    // Render structure that is incrementally built and updated. Scenes should only be updated via a render packet!
    class RenderScene
    {
    public:
        RenderScene(uint32_t max_renderable_objects);
        virtual ~RenderScene();

        // Adds a mesh pass object to the scene.
        void AddMeshPassObject(Ref<MeshPassObject> obj);

        // Removes a mesh pass object from the scene
        void RemoveMeshPassObject(Ref<MeshPassObject> obj);

        // Groups RenderableObjects by mesh & material.
        void BuildFlatBatches();

        // Flat batches are RenderableObjects grouped by mesh & material.
        std::vector<Ref<RenderableBatch>> forward_flat_batches;

        // Flat batches for the shadow pass
        std::vector<Ref<RenderableBatch>> shadow_flat_batches;

        const Ref<MeshPass>& GetShadowPass() const { return shadow_pass; }
        uint32_t GetShadowPassCount() const { return shadow_pass->count; }

        const Ref<MeshPass>& GetForwardPass() const { return forward_pass; }
        uint32_t GetForwardPassCount() const { return forward_pass->count; }
    public:
        std::vector<Ref<DirectionalLight>> directional_lights;
        std::vector<Ref<PointLight>> point_lights;

        Ref<Camera> camera;
    private:
        std::vector<Ref<MeshPass>> m_mesh_passes = {};
    private:
        // Mesh passes
        Ref<MeshPass> shadow_pass = nullptr;
    private:
        Ref<MeshPass> forward_pass = nullptr;
        Ref<MeshPass> transparent_forward_pass = nullptr;
    private:
        virtual RenderSceneId GenerateSceneId();
        std::queue<RenderSceneId> m_free_ids = {};
        RenderSceneId m_next_id = 0;
    };
}