#pragma once

#include "Math.h"

namespace yoyo
{
    // Generates an orthographic projection matrix
    YAPI Mat4x4 OrthographicProjectionMat4x4(float left, float right, float bottom, float top, float near, float far);

    // Generates an perspective projection matrix
    YAPI Mat4x4 PerspectiveProjectionMat4x4(float fov_radians, float aspect_ratio, float near, float far);

    // Generates a look at matrix 
    YAPI Mat4x4 LookAtMat4x4(const Vec3& position, const Vec3& target, const Vec3& up);

    // Generates an translation matrix
    YAPI Mat4x4 TranslationMat4x4(const Vec3& position);

    // Generates an scale matrix
    YAPI Mat4x4 ScaleMat4x4(const Vec3& scale);

    // Generates a rotation matrix an angle in radians and a rotation axis
    YAPI Mat4x4 RotateMat4x4(float angle_radians, const Vec3& axis);

    // Generates a rotation matrix given euler angles 
    YAPI Mat4x4 RotateEulerMat4x4(const Vec3& angles);

    YAPI Mat4x4 TransposeMat4x4(Mat4x4& matrix);

    YAPI Mat4x4 QuatToMat4x4(const Quat& q);

    YAPI Vec3 PositionFromMat4x4(const Mat4x4& matrix);

    YAPI Vec3 ScaleFromMat4x4(const Mat4x4& matrix);
}