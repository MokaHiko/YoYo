#pragma once

#include <Math/Math.h>
#include <Math/Quaternion.h>

#include "ECS/Entity.h"

struct TransformComponent
{
public:
    static const int MAX_CHILDREN = 200;

    yoyo::Vec3 position{ 0.0f , 0.0f, 0.0f};
    yoyo::Vec3 rotation{ 0.0f , 0.0f, 0.0f}; // Rotation in euler radians
    yoyo::Vec3 scale{ 1.0f , 1.0f, 1.0f};

    yoyo::Mat4x4 model_matrix{ 1.0f };

    const yoyo::Vec3& Forward() const; // Returns the forward component of the transform

    Entity self = {};
    Entity parent = {};
    std::array<Entity, MAX_CHILDREN> children = {};
    uint32_t children_count = 0;

    yoyo::Quat quat_rotation = {0.0f, 0.0f, 0.0f, 1.0f};
public:
    void AddChild(Entity e);
    void RemoveChild(Entity e);

    void UpdateModelMatrix();
    yoyo::Mat4x4 LocalModelMatrix();
private:
    friend class SceneGraph;
    bool dirty_flag = false;

    yoyo::Vec3 m_forward;
};

struct TagComponent
{
    std::string tag;
};