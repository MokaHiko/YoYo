#pragma once

#include <Core/Memory.h>

#include <Renderer/Material.h>
#include <Renderer/Mesh.h>
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
};

struct NewCameraComponent
{
    float time_stamp;
};

struct MeshRendererComponent
{
    MeshRendererComponent();
    ~MeshRendererComponent();

    Ref<yoyo::Mesh> mesh;
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