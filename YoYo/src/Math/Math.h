#pragma once

#include "Defines.h"
#include "MathTypes.h"

#define Y_PI 3.14159265897932

namespace yoyo
{
    YAPI float Cos(float rad);
    YAPI float Sin(float rad);
    YAPI float Tan(float rad);

    YAPI float DegToRad(float deg);
    YAPI float RadToDeg(float rad);

    YAPI const float Length(const Vec3& v1);
    YAPI const Vec3 Normalize(const Vec3& v1);

    YAPI const float Dot(const Vec3& v1, const Vec3& v2);
    YAPI const Vec3 Cross(const Vec3& v1, const Vec3& v2);

    YAPI const Vec2 operator*(const Vec2& v1,  const Vec2& v2);
    YAPI const Vec3 operator*(const Vec3& v1,  const Vec3& v2);

    YAPI const Vec2 operator+(const Vec2& v1,  const Vec2& v2);
    YAPI const Vec3 operator+(const Vec3& v1,  const Vec3& v2);

    YAPI const Vec3 operator*(const Vec3& v1,  float scalar);
    YAPI const Vec4 operator*(const Vec4& v1,  float scalar);

    YAPI const Vec3 operator/(const Vec3& v1,  float scalar);

    YAPI const Mat4x4 operator*(const Mat4x4& v1, float scalar);
    YAPI const Mat4x4 operator*(const Mat4x4& m1, const Mat4x4& m2);
}