#pragma once

#include "Defines.h"

#include "Asset.h"
#include "Mesh.h"
#include "Material.h"
#include "Animation.h"

namespace hro
{
    struct HAPI ModelInfo : public AssetInfo
    {
        ModelInfo() {}

        uint64_t bone_mapping_size = 0;  // size of bone_mapping in bytes
        uint64_t bone_matrices_size = 0;  // size of bone_matrices buffer in bytes

        // Returns the size of raw data when unpacked
        virtual const uint64_t UnpackedSize() const override 
        {
            return bone_mapping_size + bone_matrices_size;
        };

        uint64_t material_count = 0;
        std::vector<Material> materials = {};
    };

    /*
        Models hold meta data and materials as well as the compressed vertex and index data of a mesh
    */
    class HAPI Model : public Asset
    {
    public:
        // Texture from texture info
        Model() = default;
        ~Model() = default;

        // Meshes in model
        std::vector<MeshInfo> mesh_infos = {};

        // Animations used in model
        std::vector<std::string> animation_paths = {};

        // Path to skeletal mesh
        std::string skeletal_mesh_path;

        virtual void ParseInfo(AssetInfo* out) override;

        virtual void PackImpl(const AssetInfo* in, void* raw_data, size_t raw_data_size) override;
        virtual void UnpackImpl(const AssetInfo* in, void* dst_buffer) override;

        void* BoneBuffer(ModelInfo* info, void* data);
        void* VertexBoneMapBuffer(ModelInfo* info, void* data);
    };
}
