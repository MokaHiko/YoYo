#pragma once

#include "Defines.h"
#include "Math/Math.h"

namespace yoyo
{
    struct YAPI DirectionalLight
    {
        Mat4x4 view_proj;

        Vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
        Vec4 direction = Vec4{ 0.0f , 0.0f, 0.0f, 0.0f};
    };

    struct YAPI PointLight
    {
        Vec4 color;
        Vec4 position;
    };
}