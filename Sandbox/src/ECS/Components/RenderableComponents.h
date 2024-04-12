#pragma once

#include <Core/Memory.h>

#include <Renderer/Mesh.h>
#include <Renderer/SkinnedMesh.h>
#include <Renderer/Model.h>
#include <Renderer/RenderScene.h>

// Forward declarations
namespace yoyo
{
    class ParticleSystem;
    class DirectionalLight;
    class Camera;

    class Material;
}

struct DirectionalLightComponent
{
    Ref<yoyo::DirectionalLight> dir_light;
};

struct NewDirectionalLightComponent
{
    float time_stamp;
};

struct CameraComponent
{
    Ref<yoyo::Camera> camera;
    bool active;
};

struct NewCameraComponent
{
    float time_stamp;
};

struct MeshRendererComponent
{
    MeshRendererComponent(const std::string& mesh_name, const std::string& material_name);
    MeshRendererComponent();
    ~MeshRendererComponent();

    Ref<yoyo::IMesh> mesh;
    yoyo::MeshType type = yoyo::MeshType::Static;

    Ref<yoyo::Material> material;
    Ref<yoyo::MeshPassObject> mesh_object;
};

struct NewMeshComponent
{
    float time_stamp;
};

namespace yoyo{class Animator;}
struct AnimatorComponent
{
    AnimatorComponent();
    ~AnimatorComponent() = default;

    Ref<yoyo::Animator> animator;
};

struct DebugColliderRendererComponent
{
    Ref<yoyo::MeshPassObject> mesh_object;
};