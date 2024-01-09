#pragma once

#include "Defines.h"
#include "Core/Memory.h"
#include "Math/Math.h"

namespace yoyo
{
    struct VertexFormat
    {
        // 
    };

    struct YAPI Vertex
    {
        Vec3 position;
        Vec3 color;
        Vec3 normal;
        Vec2 uv;
    };

    class YAPI Mesh
    {
    public:
        Mesh();
        ~Mesh();

        virtual void Bind(void* render_context) = 0;
        virtual void Unbind() = 0;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        static Ref<Mesh> Create();

        bool draw_indexed = false;
    private:
        uint64_t m_id;
    };
}