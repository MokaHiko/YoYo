#ifndef MESH_H
#define MESH_H

#pragma once

#include "Asset.h"
#include "cassert"

#define MAX_BONES_PER_VERTEX 4

namespace hro
{
    // Vertex format all in f32
    struct HAPI Vertex_F32_PNCV
    {
        float position[3];
        float color[3];
        float normal[3];
        float uv[2];
    };

    struct HAPI Vertex_F32_PNCV_U32F32_BIW
    {
        float position[3];
        float color[3];
        float normal[3];
        float uv[2];

        uint32_t ids[MAX_BONES_PER_VERTEX];
        float weights[MAX_BONES_PER_VERTEX];
    };

    enum class VertexFormat : uint8_t
    {
        Uknown,
        F32_PNCV
    };

    enum class IndexFormat : uint8_t
    {
        Uknown,
        UINT32
    };

    struct VertexBoneData
    {
        uint32_t ids[MAX_BONES_PER_VERTEX] = { 0 };
        float weights[MAX_BONES_PER_VERTEX] = { 0.0f };

        void AddWeight(uint32_t bone_id, float weight)
        {
            // Skip bones with no weights
            if(weight == 0.0f)
            {
                return;
            };

            for (int i = 0; i < MAX_BONES_PER_VERTEX; i++)
            {
                if (weights[i] == 0.0f)
                {
                    ids[i] = bone_id;
                    weights[i] = weight;

                    return;
                }

                if (ids[i] == bone_id && weights[i] < weight)
                {
                    ids[i] = bone_id;
                    weights[i] = weight;

                    return;
                }
            }

            // printf("max bones exceeded!\n");
            //assert(0 && "Cannot add more than max bones!");
        }
    };

    struct Joint
    {
        float inverse_bind_pose_transform[16] = { 0.0f };
        char name_buffer[64]{};
    };

    struct MeshInfo
    {
        // Returns the number of vertices of this mesh
        const uint32_t GetVertexCount() const;

        // Returns the number of indices of this mesh
        const uint32_t GetIndexCount() const;

        VertexFormat vertex_format;
        uint64_t vertex_buffer_size;

        IndexFormat index_format;
        uint64_t index_buffer_size;

        uint32_t bone_count;

        std::string name;

        float model_matrix[16];
        uint32_t material_id;

        CompressionMode compression_mode;
        std::string original_file_path;
    };

    class HAPI Mesh : public Asset
    {
    public:
        // Parses the model meta data into out 
        virtual void ParseInfo(AssetInfo* out) override;
    public:
        std::string name;

        VertexFormat vertex_format;
        IndexFormat index_format;

        uint32_t bone_count;

        CompressionMode compression_mode;
        std::string original_file_path;

        std::vector<Vertex_F32_PNCV> vertices;
        std::vector<uint32_t> indices;
    protected:
        virtual void PackImpl() override;
        virtual void UnpackImpl(const AssetInfo* in, void* dst_buffer) override;
    };
}

#endif