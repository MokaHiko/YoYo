#pragma once

#include "Math/Math.h"

namespace yoyo
{
    struct DirectionalLight
    {
        Mat4x4 view_proj;

        Vec4 color;
        Vec4 direction;
    };

    struct PointLight
    {
        Vec4 color;
        Vec4 position;
    };
}