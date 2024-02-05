#pragma once

#include "Math/Math.h"

namespace yoyo
{
    struct Transform
    {
        Vec3 position {0.0f, 0.0f, 0.0f};
        Vec3 scale = {1.0f, 1.0f, 1.0f};
        Quat rotation;
    };

    struct Tag
    {
        std::string name;
    };
}