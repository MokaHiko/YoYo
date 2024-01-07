#pragma once

#include "Defines.h"
#include "Math/Math.h"

namespace yoyo
{
    struct VertexFormat
    {
        // 
    };

    YAPI struct Vertex
    {
        Vec3 position;
        Vec4 color;
        Vec3 normal;
        Vec2 uv;
    };

    YAPI class Mesh
    {
    public:
        Mesh();
        ~Mesh();

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    private:
        uint64_t m_id;
    };
}