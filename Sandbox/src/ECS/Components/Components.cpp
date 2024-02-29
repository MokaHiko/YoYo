#include "Components.h"

#include <Math/MatrixTransform.h>

#include "Core/Assert.h"
#include "ECS/EntityImpl.h"

void TransformComponent::AddChild(Entity e)
{
    YASSERT(children_count < MAX_CHILDREN, "Max child count reached!");

    // Remove from previous parent if exists
    Entity previous_parent = e.GetComponent<TransformComponent>().parent;
    if (previous_parent)
    {
        previous_parent.GetComponent<TransformComponent>().RemoveChild(e);
    }

    // Parent to self
    e.GetComponent<TransformComponent>().parent = self;

    // Add to child list
    children[children_count++] = e;
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
}

yoyo::Mat4x4 TransformComponent::LocalModelMatrix() const {

    // TODO: Matrix consecutive multiplication overload not working
    //yoyo::Mat4x4 local_model_matrix = yoyo::TranslationMat4x4(position) * yoyo::RotateEulerMat4x4(rotation) * yoyo::ScaleMat4x4(scale);
    yoyo::Mat4x4 local_model_matrix = yoyo::TranslationMat4x4(position) * yoyo::TransposeMat4x4(yoyo::QuatToMat4x4(quat_rotation)) * yoyo::ScaleMat4x4(scale);


    // TODO: switch to quaternions
    //local_model_matrix = glm::rotate(local_model_matrix, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
    //local_model_matrix = glm::rotate(local_model_matrix, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
    //local_model_matrix = glm::rotate(local_model_matrix, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));

    //local_model_matrix = glm::scale(local_model_matrix, scale);

    return local_model_matrix;
}
