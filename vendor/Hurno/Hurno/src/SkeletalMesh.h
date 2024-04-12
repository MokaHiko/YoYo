#pragma once

#include "Asset.h"
#include "Mesh.h"

namespace hro
{
    const int MAX_NODE_CHILDREN = 64;
    const int INVALID_JOINT_INDEX = -1;

    class HAPI Node
    {
    public:
        int joint_index = INVALID_JOINT_INDEX;

        // The transformation relative to node's parent
        float transformation[16];

        char name_buffer[32];

        void AddChild(Node* child)
        {
            assert(child && "Cannot add NULL child!");
            assert(m_children_count < MAX_NODE_CHILDREN && "Max child nodes reached!");
            m_children[m_children_count++] = child->m_ptr;
        }

        Node* GetChild(int child_index, void* data) const 
        {
            assert(data && "Cannot parse invalid data");
            assert(child_index < m_children_count && "Child out of range");

            uint32_t offset = m_children[child_index];
            return (Node*)((char*)data + offset);
        }

        uint32_t ChildrenCount() const {return m_children_count;}
    private:
        friend class SkeletalMesh;
        uint32_t m_children[MAX_NODE_CHILDREN] = {}; // Offsets into data buffer
        uint32_t m_children_count = 0;
        uint32_t m_ptr = 0;
    };

    struct HAPI SkeletalMeshInfo: public AssetInfo
    {
        // Returns the size of raw data when unpacked
        virtual const uint64_t UnpackedSize() const { return 0; }
    };

    class HAPI SkeletalMesh : public Asset
    {
    public:
        SkeletalMesh() = default;
        ~SkeletalMesh() = default;

        virtual void ParseInfo(AssetInfo* out) override;

        Node* CreateNode();

        const uint32_t RawDataSize() const { return m_insert_index; }
        Node* Root() { return (Node*)m_data; }

        void Allocate(size_t size){m_data = malloc(size);}
    protected:
        virtual void PackImpl(const AssetInfo* info) override;
        virtual void UnpackImpl(const AssetInfo* in, void* dst_buffer) override;
    private:
        uint32_t m_insert_index = 0;
        void* m_data = nullptr;
    };
}
