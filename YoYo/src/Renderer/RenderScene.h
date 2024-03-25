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
        RenderScene();
        virtual ~RenderScene();

        // Adds a mesh pass object to the scene.
        void AddMeshPassObject(Ref<MeshPassObject> obj);

        // Groups RenderableObjects by mesh & material.
        void BuildFlatBatches(const std::vector<Ref<MeshPassObject>>& objs);

        // Flat batches are RenderableObjects grouped by mesh & material.
        std::vector<Ref<RenderableBatch>> forward_flat_batches;
    public:
        std::vector<Ref<DirectionalLight>> directional_lights;
        std::vector<Ref<PointLight>> point_lights;

        Ref<Camera> camera;

        Ref<MeshPass> shadow_pass;
        Ref<MeshPass> forward_pass;
        Ref<MeshPass> transparent_forward_pass;
    private:
        virtual RenderSceneId GenerateSceneId();
        RenderSceneId m_next_id = 0;
    };
}