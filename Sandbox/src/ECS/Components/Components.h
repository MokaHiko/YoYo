#pragma once

#include <Math/Math.h>

struct TransformComponent
{
    yoyo::Vec3 position{ 0.0f, 0.0f, 0.0f };
    yoyo::Vec3 scale = { 1.0f, 1.0f, 1.0f };
    yoyo::Vec3 rotation = { 0.0f, 0.0f, 0.0f };

    yoyo::Quat quat_rotation;

    void UpdateModelMatrix();

    yoyo::Mat4x4 model_matrix;
};

struct TagComponent
{
    std::string tag;
};