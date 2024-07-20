#pragma once

#include "Defines.h"

#include "Math/Math.h"
#include "Core/Memory.h"
#include "Core/Assert.h"

#include "Resource/Resource.h"
#include "Renderer/RenderTypes.h"

namespace yoyo
{
    class YAPI IMesh;
}

template <>
struct std::hash<yoyo::IMesh>;

namespace yoyo
{
    enum class VertexInputRate
    {
        Vertex = 0,
        Instance = 1,

        Maximum
    };

    // Default Vertex_PCNVT used for static meshes.
    struct YAPI Vertex
    {
        Vec3 position;
        Vec3 color;

        Vec3 normal;

        Vec2 uv;
        Vec3 tangent;
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
        Uncached = 1,              // Not cached in runtime resource manager
        Unuploaded = 1 << 2,       // Mesh not in gpu
        VertexDataChange = 1 << 3, // Vertex data not updated
        IndexDataChange = 1 << 4,  // Index data not updated
        BoneDataChanged = 1 << 5,  // Index data not updated
    };

    inline MeshDirtyFlags operator~(MeshDirtyFlags a) { return (MeshDirtyFlags) ~(int)a; }
    inline MeshDirtyFlags operator|(MeshDirtyFlags a, MeshDirtyFlags b) { return (MeshDirtyFlags)((int)a | (int)b); }
    inline MeshDirtyFlags operator&(MeshDirtyFlags a, MeshDirtyFlags b) { return (MeshDirtyFlags)((int)a & (int)b); }
    inline MeshDirtyFlags operator^(MeshDirtyFlags a, MeshDirtyFlags b) { return (MeshDirtyFlags)((int)a ^ (int)b); }
    inline MeshDirtyFlags &operator|=(MeshDirtyFlags &a, MeshDirtyFlags b) { return (MeshDirtyFlags &)((int &)a |= (int)b); }
    inline MeshDirtyFlags &operator&=(MeshDirtyFlags &a, MeshDirtyFlags b) { return (MeshDirtyFlags &)((int &)a &= (int)b); }
    inline MeshDirtyFlags &operator^=(MeshDirtyFlags &a, MeshDirtyFlags b) { return (MeshDirtyFlags &)((int &)a ^= (int)b); }

    template <typename VertexType, typename IndexType>
    class YAPI Mesh;
    class YAPI MeshFactory
    {
    public:
        template <typename VertexType, typename IndexType>
        using MeshCreateFunc = std::function<Ref<Mesh<VertexType, IndexType>>(const std::string &)>;

        template <typename VertexType, typename IndexType>
        static void Register(const std::string &type, MeshCreateFunc<VertexType, IndexType> func)
        {
            GetRegistry<VertexType, IndexType>()[type] = func;
        }

        template <typename VertexType, typename IndexType>
        static Ref<Mesh<VertexType, IndexType>> Create(const std::string &type, const std::string &name)
        {
            auto &registry = GetRegistry<VertexType, IndexType>();
            if (registry.find(type) == registry.end())
            {
                YASSERT(0, "Mesh type not registered.");
            }
            return registry[type](name);
        }

    private:
        template <typename VertexType, typename IndexType>
        static std::unordered_map<std::string, MeshCreateFunc<VertexType, IndexType>> &GetRegistry()
        {
            static std::unordered_map<std::string, MeshCreateFunc<VertexType, IndexType>> registry;
            return registry;
        }
    };

    class YAPI IMesh : public Resource
    {
    public:
        RESOURCE_TYPE(IMesh)

        virtual void Bind(void *render_context) = 0;
        virtual void Unbind() {};

        virtual void UploadMeshData(bool free_host_memory = false) = 0;
        virtual uint64_t Hash() const = 0;

        const virtual uint32_t GetIndexCount() const = 0;
        const virtual uint32_t GetVertexCount() const = 0;

        virtual void SetMeshType(MeshType type) = 0;
        virtual const MeshType GetMeshType() const = 0;

        virtual const uint64_t GetVertexSize() const = 0;
        virtual const uint64_t GetIndexSize() const = 0;

        virtual const MeshDirtyFlags DirtyFlags() const = 0;
    };

    template <typename VertexType, typename IndexType = uint32_t>
    class YAPI Mesh : public IMesh
    {
    public:
        RESOURCE_TYPE(Mesh)

        Mesh()
        {
            // Default flags
            AddDirtyFlags(MeshDirtyFlags::Unuploaded);
        };
        virtual ~Mesh() = default;

        const std::vector<VertexType> &GetVertices() const { return vertices; }
        const std::vector<IndexType> &GetIndices() const { return indices; }

        std::vector<VertexType> &GetVertices()
        {
            m_dirty |= MeshDirtyFlags::VertexDataChange;
            return vertices;
        }

        std::vector<IndexType> &GetIndices()
        {
            m_dirty |= MeshDirtyFlags::IndexDataChange;
            return indices;
        }

        virtual uint64_t Hash() const override
        {
            //return std::hash<IMesh>{}(*this);
            // TODO: Properly use hash function
            return rand();
        }

        // Implemented by backend render api
        static Ref<Mesh<VertexType, IndexType>> CreateImpl(const std::string &type, const std::string &name = "")
        {
            return MeshFactory::Create<VertexType, IndexType>(type, name);
        }

        // Implemented by backend render api
        static Ref<Mesh<VertexType, IndexType>> CreateFromBuffersImpl(const std::string &name, void *vertex_buffer, uint64_t vertex_buffer_size, void *index_buffer, uint64_t index_buffer_size)
        {
            Ref<Mesh<VertexType, IndexType>> mesh = Mesh<VertexType, IndexType>::CreateImpl(name);
            mesh->vertices.resize(vertex_buffer_size / sizeof(VertexType));
            memcpy(mesh->vertices.data(), (char *)vertex_buffer, vertex_buffer_size);

            mesh->indices.resize(index_buffer_size / sizeof(IndexType));
            memcpy(mesh->indices.data(), (char *)index_buffer, index_buffer_size);

            return mesh;
        }

    public:
        const virtual uint32_t GetIndexCount() const override final { return static_cast<uint32_t>(static_cast<uint32_t>(indices.size())); }
        const virtual uint32_t GetVertexCount() const override final { return static_cast<uint32_t>(static_cast<uint32_t>(vertices.size())); }

        virtual void SetMeshType(MeshType type) override final { m_type = type; }
        virtual const MeshType GetMeshType() const override final { return m_type; }

        virtual const uint64_t GetVertexSize() const override final { return sizeof(VertexType); }
        virtual const uint64_t GetIndexSize() const override final { return sizeof(IndexType); }

        virtual const MeshDirtyFlags DirtyFlags() const override { return m_dirty; }
        virtual void RecalculateNormals() {}; // Recalculates normals of mesh
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
        StaticMesh()
        {
            SetMeshType(MeshType::Static);
        };
        virtual ~StaticMesh() = default;

        static Ref<StaticMesh> Create(const std::string &name = "");
        static Ref<StaticMesh> LoadFromAsset(const char *asset_path, const std::string &name = "");
    };
}

template <>
struct std::hash<yoyo::IMesh>
{
    std::size_t operator()(const yoyo::IMesh &mesh) const noexcept
    {
        // http://stackoverflow.com/a/1646913/126995
        std::size_t res = mesh.GetVertexCount() + mesh.GetIndexCount();
        res = res * 31 + hash<string>()(mesh.name);

        return res;
    }
};