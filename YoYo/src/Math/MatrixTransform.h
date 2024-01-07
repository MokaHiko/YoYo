#pragma once

#include "MathTypes.h"

namespace yoyo
{
    // Generates an orthographic projection matrix
    YAPI Mat4x4 OrthographicProjectionMat4x4(float left, float right, float bottom, float top, float near, float far);

    // Generates an perspective projection matrix
    YAPI Mat4x4 PerspectiveProjectionMat4x4(float fov_radians, float aspect_ratio, float near, float far);

    // Generates an translation matrix
    YAPI Mat4x4 TranslationMat4x4(const Vec3& position);

    // Generates an scale matrix
    YAPI Mat4x4 ScaleMat4x4(const Vec3& scale);

    YAPI Mat4x4 RotateMat4x4(float angle_radians, const Vec3& axis);

    YAPI Mat4x4 TransposeMat4x4(Mat4x4& matrix);
}