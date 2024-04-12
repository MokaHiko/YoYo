#pragma once

#include "Defines.h"

#include "Math/Math.h"
#include "Core/Memory.h"

#include "Resource/Resource.h"
#include "Renderer/RenderTypes.h"

namespace yoyo
{
    enum class VertexInputRate
    {
        Vertex = 0,
        Instance = 1,

        Maximum
    };

    struct YAPI Vertex
    {
        Vec3 position;
        Vec3 color;
        Vec3 normal;

        Vec2 uv;
    };

    struct sample
    {
        int nums[4];
    };

    enum class MeshType
    {
        Uknown,
        Static,
        Skinned,
    };

    enum class MeshDirtyFlags
    {
        Clean = 0,
        Uncached = 1,   // Not cached in runtime resource manager
        Unuploaded = 1 << 2, // Mesh not in gpu
        VertexDataChange = 1 << 3, // Vertex data not updated
        IndexDataChange = 1 << 4, // Index data not updated
        BoneDataChanged = 1 << 5, // Index data not updated
    };

    inline MeshDirtyFlags operator~ (MeshDirtyFlags a) { return (MeshDirtyFlags)~(int)a; }
    inline MeshDirtyFlags operator| (MeshDirtyFlags a, MeshDirtyFlags b) { return (MeshDirtyFlags)((int)a | (int)b); }
    inline MeshDirtyFlags operator& (MeshDirtyFlags a, MeshDirtyFlags b) { return (MeshDirtyFlags)((int)a & (int)b); }
    inline MeshDirtyFlags operator^ (MeshDirtyFlags a, MeshDirtyFlags b) { return (MeshDirtyFlags)((int)a ^ (int)b); }
    inline MeshDirtyFlags& operator|= (MeshDirtyFlags& a, MeshDirtyFlags b) { return (MeshDirtyFlags&)((int&)a |= (int)b); }
    inline MeshDirtyFlags& operator&= (MeshDirtyFlags& a, MeshDirtyFlags b) { return (MeshDirtyFlags&)((int&)a &= (int)b); }
    inline MeshDirtyFlags& operator^= (MeshDirtyFlags& a, MeshDirtyFlags b) { return (MeshDirtyFlags&)((int&)a ^= (int)b); }

    class IMesh
    {
    public:
        virtual void Bind(void* render_context) = 0;
        virtual void Unbind() {};

        virtual void UploadMeshData(bool free_host_memory = false) = 0;
        virtual uint64_t Hash() const = 0;

        const virtual uint32_t GetIndexCount() const = 0;
        const virtual uint32_t GetVertexCount() const = 0;

        virtual void SetMeshType(MeshType type) = 0;
        virtual const MeshType GetMeshType() const = 0;

        virtual const uint64_t GetVertexSize() const = 0;
        virtual const uint64_t GetIndexSize() const = 0;
    };

    template<typename VertexType, typename IndexType = uint32_t>
    class Mesh : public IMesh, public Resource
    {
    public:
        Mesh()
        {
            // Default flags
            AddDirtyFlags(MeshDirtyFlags::Unuploaded);
        };

        virtual ~Mesh() = default;

        const std::vector<VertexType>& GetVertices() const { return vertices; }
        const std::vector<IndexType>& GetIndices() const { return indices; }

        std::vector<VertexType>& GetVertices()
        {
            m_dirty |= MeshDirtyFlags::VertexDataChange;
            return vertices;
        }

        std::vector<IndexType>& GetIndices()
        {
            m_dirty |= MeshDirtyFlags::IndexDataChange;
            return indices;
        }
    public:
        const virtual uint32_t GetIndexCount() const override final { return indices.size(); }
        const virtual uint32_t GetVertexCount() const override final { return vertices.size(); }

        virtual void SetMeshType(MeshType type) override final { m_type = type; }
        virtual const MeshType GetMeshType() const override final { return m_type; }

        virtual const uint64_t GetVertexSize() const override final { return sizeof(VertexType); }
        virtual const uint64_t GetIndexSize() const override final { return sizeof(IndexType); }

        virtual void RecalculateNormals() {}; // Recalculates normals of mesh
        MeshDirtyFlags DirtyFlags() { return m_dirty; }
    protected:
        void AddDirtyFlags(MeshDirtyFlags flags_bits) { m_dirty |= flags_bits; }
        void RemoveDirtyFlags(MeshDirtyFlags flags_bits) { m_dirty &= ~flags_bits; }

        std::vector<VertexType> vertices = {};
        std::vector<IndexType> indices = {};
    private:
        MeshDirtyFlags m_dirty = MeshDirtyFlags::Clean;
        MeshType m_type = MeshType::Uknown;
    };

    class YAPI StaticMesh : public Mesh<yoyo::Vertex, uint32_t>
    {
    public:
        RESOURCE_TYPE(StaticMesh)
        StaticMesh()
        {
            SetMeshType(MeshType::Static);
        };
        virtual ~StaticMesh() = default;

        static Ref<StaticMesh> Create(const std::string& name = "");
	    static Ref<StaticMesh> CreateFromBuffers(const std::string& name, void* vertex_buffer, uint64_t vertex_buffer_size, void* index_buffer, uint64_t index_buffer_size);
        static Ref<StaticMesh> LoadFromAsset(const char* asset_path, const std::string& name = "");

        virtual uint64_t Hash() const override;
    };
}

template<>
struct std::hash<yoyo::StaticMesh>
{
    std::size_t operator()(const yoyo::StaticMesh& mesh) const noexcept
    {
        // http://stackoverflow.com/a/1646913/126995
        std::size_t res = mesh.GetVertices().size() + mesh.GetIndices().size();
        res = res * 31 + hash<string>()(mesh.name);

        return res;
    }
};
