#pragma once

#include "Math.h"

namespace yoyo
{
    union YAPI Quat
    {
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

        Quat& operator*=(const Quat &other);
    };
    // Returns the normal of the quaternion.
    YAPI float NormalQuat(const Quat& q);

    // Returns a normalized version of the quaternion.
    YAPI Quat NormalizeQuat(const Quat& q);

    // Returns a quaternion from the given axis and angle normalized by deafult.
    YAPI Quat QuatFromAxisAngle(const Vec3& axis, float angle, bool normalize = true);

    // Returns the euler angles of the quaternion.
    YAPI Vec3 EulerAnglesFromQuat(const Quat& q);

    YAPI const Quat operator*(const Quat& q1, const Quat& q2);
}
