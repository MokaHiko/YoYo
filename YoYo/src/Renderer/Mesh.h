#pragma once

#include "Defines.h"

#include "Core/Memory.h"
#include "Math/Math.h"

#include "Resource/Resource.h"

namespace yoyo
{
    enum class VertexFormat
    {
        P3c3n3u2,
        P4c4n3u2,
    };

    struct YAPI Vertex
    {
        Vec3 position;
        Vec3 color;
        Vec3 normal;

        Vec2 uv;
    };

    enum class MeshDirtyFlags
    {
        Clean = 0,
        Uncached = 1,   // Not cached in runtime resource manager
        Unuploaded = 1 << 2, // Mesh not in gpu
        VertexDataChange = 1 << 3, // Vertex data not updated
        IndexDataChange = 1 << 4, // Index data not updated
    };

    inline MeshDirtyFlags operator~ (MeshDirtyFlags a) { return (MeshDirtyFlags)~(int)a; }
    inline MeshDirtyFlags operator| (MeshDirtyFlags a, MeshDirtyFlags b) { return (MeshDirtyFlags)((int)a | (int)b); }
    inline MeshDirtyFlags operator& (MeshDirtyFlags a, MeshDirtyFlags b) { return (MeshDirtyFlags)((int)a & (int)b); }
    inline MeshDirtyFlags operator^ (MeshDirtyFlags a, MeshDirtyFlags b) { return (MeshDirtyFlags)((int)a ^ (int)b); }
    inline MeshDirtyFlags& operator|= (MeshDirtyFlags& a, MeshDirtyFlags b) { return (MeshDirtyFlags&)((int&)a |= (int)b); }
    inline MeshDirtyFlags& operator&= (MeshDirtyFlags& a, MeshDirtyFlags b) { return (MeshDirtyFlags&)((int&)a &= (int)b); }
    inline MeshDirtyFlags& operator^= (MeshDirtyFlags& a, MeshDirtyFlags b) { return (MeshDirtyFlags&)((int&)a ^= (int)b); }

    class YAPI Mesh
    {
    public:
        Mesh() = default;
        virtual ~Mesh() = default;

        virtual void Bind(void* render_context) = 0;
        virtual void Unbind() = 0;

        virtual void UploadMeshData(bool free_host_memory = false) = 0;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        void RecalculateNormals(); // Recalculates normals of mesh

        static Ref<Mesh> Create(const std::string& name = "");

        const ResourceId& ID() const { return m_id; }
        MeshDirtyFlags DirtyFlags() { return m_dirty; }
    protected:
        MeshDirtyFlags m_dirty;
        ResourceId m_id;
    };
}