#pragma once

#include <Core/Memory.h>

#include <Renderer/Material.h>
#include <Renderer/Mesh.h>
#include <Renderer/SkinnedMesh.h>
#include <Renderer/Model.h>
#include <Renderer/Light.h>
#include <Renderer/Camera.h>

#include <Renderer/RenderScene.h>

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

struct ModelRendererComponent
{

};

namespace yoyo{class Animator;}
struct AnimatorComponent
{
    AnimatorComponent();
    ~AnimatorComponent() = default;

    Ref<yoyo::Animator> animator;
};