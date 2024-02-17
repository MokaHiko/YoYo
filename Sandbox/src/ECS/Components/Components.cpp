#include "Components.h"

#include <Math/MatrixTransform.h>

void TransformComponent::UpdateModelMatrix() 
{
    model_matrix = yoyo::TranslationMat4x4(position) * yoyo::RotateEulerMat4x4(rotation) * yoyo::ScaleMat4x4(scale);
}
