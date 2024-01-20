#pragma once

#include "Defines.h"
#include "MathTypes.h"

#define Y_PI 3.14159265897932

/*
    By Default the math library assumes column major
*/

namespace yoyo
{
    const Vec2 operator*(const Vec2& v1,  const Vec2& v2);
    const Vec3 operator*(const Vec3& v1,  const Vec3& v2);

    const Vec2 operator+(const Vec2& v1,  const Vec2& v2);
    const Vec3 operator+(const Vec3& v1,  const Vec3& v2);

    const Vec4 operator*(const Vec4& v1,  float scalar);

    const Mat4x4 operator*(const Mat4x4& v1, float scalar);
    const Mat4x4 operator*(const Mat4x4& m1, const Mat4x4& m2);
}