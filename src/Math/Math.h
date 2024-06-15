#pragma once

#include "Defines.h"
#include "MathTypes.h"

#define Y_PI 3.14159265897932

namespace yoyo
{
    YAPI float Cos(float rad);
    YAPI float Sin(float rad);
    YAPI float Tan(float rad);

    YAPI float ACos(float rad);

    YAPI float DegToRad(float deg);
    YAPI float RadToDeg(float rad);

    YAPI const float Lerp(float a, float b, float t);
    YAPI const float Clamp(float value, float min, float max);

    YAPI bool FloatCompare(float x, float y, float epsilon = 0.01f);

    YAPI const float SquaredLength(const Vec3& v1);
    YAPI const float Length(const Vec2& v1);
    YAPI const float Length(const Vec3& v1);
    YAPI const float Length(const Vec4& v1);
    YAPI const Vec2 Normalize(const Vec2& v1);
    YAPI const Vec3 Normalize(const Vec3& v1);
    YAPI const Vec4 Normalize(const Vec4& v1);
    YAPI const Vec2 NormalizeOrZero(const Vec2& v1);

    YAPI const float Dot(const Vec3& v1, const Vec3& v2);
    YAPI const float Dot(const Vec4& v1, const Vec4& v2);

    YAPI const Vec3 Cross(const Vec3& v1, const Vec3& v2);

    YAPI const Vec3 Lerp(const Vec3& v1, const Vec3& v2, float t);
    YAPI const Vec4 Lerp(const Vec4& v1, const Vec4& v2, float t);

    YAPI const Vec2 operator*(const Vec2& v1, const Vec2& v2);
    YAPI const Vec3 operator*(const Vec3& v1, const Vec3& v2);

    YAPI const Vec2 operator+(const Vec2& v1, const Vec2& v2);
    YAPI const Vec3 operator+(const Vec3& v1, const Vec3& v2);
    YAPI const IVec2 operator+(const IVec2& v1, const IVec2& v2);

    YAPI const Vec2 operator-(const Vec2& v1, const Vec2& v2);
    YAPI const Vec3 operator-(const Vec3& v1, const Vec3& v2);
    YAPI const IVec2 operator-(const IVec2& v1, const IVec2& v2);

    YAPI const IVec2 operator*(const IVec2& v1, int scalar);
    YAPI const IVec3 operator*(const IVec3& v1, int scalar);
    YAPI const Vec3 operator*(const Vec3& v1, float scalar);
    YAPI const Vec4 operator*(const Vec4& v1, float scalar);

    YAPI const Vec2 operator/(const Vec2& v1, float scalar);
    YAPI const Vec3 operator/(const Vec3& v1, float scalar);
    YAPI const Vec4 operator/(const Vec4& v1, float scalar);

    YAPI const Mat4x4 operator*(const Mat4x4& v1, float scalar);
    YAPI const Mat4x4 operator*(const Mat4x4& m1, const Mat4x4& m2);

    YAPI yoyo::Vec3 operator*(const Mat4x4& m, const yoyo::Vec3& v);
    YAPI const Vec4 operator*(const Mat4x4& m, const Vec4& v);
}