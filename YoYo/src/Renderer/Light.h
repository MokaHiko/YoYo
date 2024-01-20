#pragma once

#include "Math/Math.h"

namespace yoyo
{
    struct DirectionalLight
    {
        Vec4 color;
        Vec4 direction;
        Vec4 position;
    };

    struct PointLight
    {
        Vec4 color;
        Vec4 position;
    };
}