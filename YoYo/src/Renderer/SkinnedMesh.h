#pragma once

#include "Mesh.h"

namespace yoyo
{
    struct YAPI SkinnedVertex 
    {
        Vec3 position = { 0.0f, 0.0f, 0.0f };
        Vec3 color = { 0.0f, 0.0f, 0.0f };
        Vec3 normal = { 0.0f, 0.0f, 0.0f };

        Vec2 uv = { 0.0f, 0.0f};

        int bone_ids[4] = { 0, 0, 0, 0 };
        float bone_weights[4] = {0.0f, 1.0f, 0.0f, 0.0f};
    };

    struct YAPI SkinnedMeshJoint
    {
        std::string name = "";

        // Joints transform in mesh space in bind pose
        Mat4x4 bind_pose_transform = {};
        Mat4x4 inverse_bind_pose_transform = {};

        // Index into skinned mesh bones vector
        uint32_t bone_id;
    };

    struct YAPI SkeletalNode
    {
        int joint_index = -1;
        std::vector<SkeletalNode*> children;
        std::string name = "";

        // The transform of the node relative to the parent
        yoyo::Mat4x4 transform;
    };

    // A hierarchy of joints
    struct YAPI SkeletalHierarchy : public Resource
    {
        RESOURCE_TYPE(SkeletalHierarchy);
        static Ref<SkeletalHierarchy> Create(const std::string& name = "");
        static Ref<SkeletalHierarchy> LoadFromAsset(const char* asset_path, const std::string& name = "");

        void Traverse(std::function<void(const SkeletalNode*)> fn) const;
        const SkeletalNode* GetRoot() const {return root;}
    private:
        const void TraverseRecursive(const SkeletalNode* node, std::function<void(const SkeletalNode*)> fn) const;
        SkeletalNode* root = nullptr;
    };

    class YAPI SkinnedMesh : public Mesh<yoyo::SkinnedVertex, uint32_t>
    {
    public:
        SkinnedMesh()
        {
            SetMeshType(MeshType::Skinned);
        };
        virtual ~SkinnedMesh() = default;

        static Ref<SkinnedMesh> Create(const std::string& name = "");
        virtual uint64_t Hash() const override;

        std::vector<SkinnedMeshJoint> joints;
        Ref<SkeletalHierarchy> skeletal_hierarchy;
        std::vector<Mat4x4> bones;
    };
}

template<>
struct std::hash<yoyo::SkinnedMesh>
{
    std::size_t operator()(const yoyo::SkinnedMesh& mesh) const noexcept
    {
        // http://stackoverflow.com/a/1646913/126995
        std::size_t res = mesh.vertices.size() + mesh.indices.size();
        res = res * 31 + hash<string>()(mesh.name);
        res = res * 31 + hash<uint64_t>()(mesh.bones.size());

        return res;
    }
};

