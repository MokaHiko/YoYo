#pragma once

#include "Math.h"

namespace yoyo
{
    // Returns the normal of the quaternion 
    YAPI float NormalQuat(const Quat& q);

    // Returns a normalized version of the quaternion
    YAPI Quat NormalizeQuat(const Quat& q);

    YAPI Quat QuatFromAxisAngle(Vec3 axis, float angle, bool normalize = true);

    YAPI const Quat operator*(const Quat& q1, const Quat& q2);
}
