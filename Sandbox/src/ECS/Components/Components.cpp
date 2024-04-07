#include "Components.h"

#include <Math/Quaternion.h>
#include <Math/MatrixTransform.h>

#include "Core/Assert.h"
#include "ECS/EntityImpl.h"

const yoyo::Vec3& TransformComponent::Forward() const
{
    return m_forward;
}

void TransformComponent::AddChild(Entity e)
{
    YASSERT(children_count < MAX_CHILDREN, "Max child count reached!");

    // Remove from previous parent if exists
    Entity previous_parent = e.GetComponent<TransformComponent>().parent;
    if (previous_parent)
    {
        previous_parent.GetComponent<TransformComponent>().RemoveChild(e);
    }

    // Parent to self and update 
    e.GetComponent<TransformComponent>().parent = self;

    // Add to child list
    children[children_count++] = e;

    // Update self before updating new child
    UpdateModelMatrix();
    e.GetComponent<TransformComponent>().UpdateModelMatrix();
}

void TransformComponent::RemoveChild(Entity e)
{
    int insert_index = -1;
    for (uint32_t i = 0; i < children_count; i++)
    {
        if (children[i] == e)
        {
            insert_index = i;
            break;
        }
    }

    if (insert_index < 0)
    {
        YASSERT("Child is not parented by object!");
        return;
    }

    int back_index = children_count - 1;
    if (back_index > -1)
    {
        Entity back = children[back_index];
        if (back != e)
        {
            children[insert_index] = back;
        }
    }
    children_count--;

    auto& child_transform = e.GetComponent<TransformComponent>();
    child_transform.parent = {};

    // TODO: Optionally Parent child back to root
    //e.GetComponent<TransformComponent>().parent = m_scen->Root();
}

void TransformComponent::UpdateModelMatrix()
{
    if (parent)
    {
        model_matrix = parent.GetComponent<TransformComponent>().model_matrix * LocalModelMatrix();
    }
    else
    {
        model_matrix = LocalModelMatrix();
    }

    // TODO: Clamp
    // Update euler angles
    rotation = yoyo::EulerAnglesFromQuat(yoyo::NormalizeQuat(quat_rotation));
    // if(rotation.x < 0)
    // {
    //     rotation.x = 180 - rotation.x;
    // }
    // if(rotation.y < 0)
    // {
    //     rotation.y = 180 - rotation.y;
    // }
}

yoyo::Mat4x4 TransformComponent::LocalModelMatrix() {
    m_local_scale_matrix = yoyo::ScaleMat4x4(scale);
    m_local_rotation_matrix = yoyo::TransposeMat4x4(yoyo::QuatToMat4x4(quat_rotation));
    m_local_translation_matrix = yoyo::TranslationMat4x4(position);

    m_forward = yoyo::Normalize(m_local_rotation_matrix * yoyo::Vec3{0.0f, 0.0f, 1.0f});
    yoyo::Mat4x4 local_model_matrix = m_local_translation_matrix * m_local_rotation_matrix * m_local_scale_matrix;

    return local_model_matrix;
}


void TransformComponent::SetGlobalTransform(const yoyo::Mat4x4 transform_matrix)
{
    if(!parent)
    {
        scale = yoyo::ScaleFromMat4x4(transform_matrix);
        quat_rotation = yoyo::RotationFromMat4x4(transform_matrix);
        position = yoyo::PositionFromMat4x4(transform_matrix);

        YASSERT(0);
        return;
    }

    yoyo::Mat4x4 local_matrix = yoyo::InverseMat4x4(parent.GetComponent<TransformComponent>().model_matrix) * transform_matrix;

	scale = yoyo::ScaleFromMat4x4(local_matrix);
	quat_rotation = yoyo::RotationFromMat4x4(local_matrix);
	position = yoyo::PositionFromMat4x4(local_matrix);

    UpdateModelMatrix();
}

void TransformComponent::SetLocalTransform(const yoyo::Mat4x4 transform) 
{
	scale = yoyo::ScaleFromMat4x4(transform);
	quat_rotation = yoyo::RotationFromMat4x4(transform);
	position = yoyo::PositionFromMat4x4(transform);

    UpdateModelMatrix();
}
