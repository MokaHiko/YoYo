#pragma once

#include "Math.h"

namespace yoyo
{
    union YAPI Quat
    {
        Quat() : x(0.0f), y(0.0f), z(0.0f), w(1.0f), elements{0.0f, 0.0f, 0.0f, 1.0f} {} // Initialize elements in the default constructor
        ~Quat() = default;

        Quat(float v_x, float v_y, float v_z, float scalar)
            : x(v_x), y(v_y), z(v_z), w(scalar), elements{v_x, v_y, v_z, scalar} {} // Initialize elements in the parameterized constructor

        Quat(const Vec3 &v, float scalar)
            : x(v.x), y(v.y), z(v.z), w(scalar), elements{v.x, v.y, v.z, scalar} {}
#ifdef YUSESIMD
        // Use SIMD
        alignas(16) _m128 data;
#endif
        alignas(16) float elements[4];
        struct
        {
            union
            {
                float x, r, s;
            };
            union
            {
                float y, g, t;
            };
            union
            {
                float z, b, p;
            };
            union
            {
                float w, a, q;
            };
        };

        operator Vec4() const { return {x, y, z, w}; }
        Quat &operator*=(const Quat &other);
    };
    // Returns the normal of the quaternion.
    YAPI float NormalQuat(const Quat &q);

    // Returns a normalized version of the quaternion.
    YAPI Quat NormalizeQuat(const Quat &q);

    // Returns a quaternion from the given axis and angle normalized by deafult.
    YAPI Quat QuatFromAxisAngle(const Vec3 &axis, float angle, bool normalize = true);

    // Returns the euler angles of the quaternion.
    YAPI Vec3 EulerAnglesFromQuat(const Quat &q);

    YAPI const Quat operator*(const Quat &q1, const Quat &q2);

    YAPI const Quat Slerp(const Quat &q1, const Quat &q2, float t);
    YAPI const Quat operator*(const Quat &q, float scalar);
    YAPI const Quat operator+(const Quat &q1, const Quat &q2);
    YAPI const Quat operator-(const Quat &q1, const Quat &q2);
}
