#pragma once

#include "Core/Memory.h"
#include "Math/Math.h"

#include "Light.h"
#include "Camera.h"

namespace yoyo
{
    class Mesh;
    class Material;
    struct MeshRenderer
    {
        Ref<Mesh> mesh;
        Ref<Material> material;
    };

    struct MeshPassObject
    {
        Ref<Mesh> mesh;
        Ref<Material> material;
        Mat4x4 model_matrix;
        uint32_t id;
    };

    // Ex. forward pass, shadow pass, 
    struct MeshPass
    {
    public:
        std::vector<Ref<MeshPassObject>> renderables;
        uint32_t pass_id;
    };

    // Render structure that is incrementally built and updated
    class RenderScene
    {
    public:
        RenderScene();
        virtual ~RenderScene();
    public:
        std::vector<Ref<DirectionalLight>> directional_lights;
        std::vector<Ref<PointLight>> point_lights;
        Ref<Camera> main_camera;

        Ref<MeshPass> shadow_pass;
        Ref<MeshPass> forward_pass;
        Ref<MeshPass> transparent_forward_pass;
    };
}