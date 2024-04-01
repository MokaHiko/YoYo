#pragma once

#include <Math/Math.h>
#include <Math/Quaternion.h>

#include "ECS/Entity.h"

struct TransformComponent
{
public:
    static const int MAX_CHILDREN = 200;

    TransformComponent() = default;
    ~TransformComponent() = default;
    
    yoyo::Vec3 position{ 0.0f , 0.0f, 0.0f};
    yoyo::Vec3 rotation{ 0.0f , 0.0f, 0.0f}; // Rotation in euler radians
    yoyo::Vec3 scale{ 1.0f , 1.0f, 1.0f};

    yoyo::Mat4x4 model_matrix{};

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

    void SetGlobalTransform(const yoyo::Mat4x4 transform);
    void SetLocalTransform(const yoyo::Mat4x4 transform);

    const yoyo::Mat4x4& LocalTranslationMatrix() const {return m_local_translation_matrix;}
    const yoyo::Mat4x4& LocalRotationMatrix() const {return m_local_rotation_matrix;}
    const yoyo::Mat4x4& LocalScaleMatrix() const {return m_local_scale_matrix;}

    yoyo::Mat4x4& LocalTranslationMatrix() {return m_local_translation_matrix;}
    yoyo::Mat4x4& LocalRotationMatrix() {return m_local_rotation_matrix;}
    yoyo::Mat4x4& LocalScaleMatrix() {return m_local_scale_matrix;}

    void SetLocalTranslationMatrix(const yoyo::Mat4x4& mat) {m_local_translation_matrix = mat;}
private:
    friend class SceneGraph;
    bool dirty_flag = false;

    yoyo::Mat4x4 m_local_translation_matrix{};
    yoyo::Mat4x4 m_local_rotation_matrix{};
    yoyo::Mat4x4 m_local_scale_matrix{};

    yoyo::Vec3 m_forward;
};

struct TagComponent
{
    std::string tag;
};