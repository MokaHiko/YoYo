#pragma once

#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/SkinnedMesh.h"

#include "VulkanStructures.h"

const int MESH_SKINNED_BONES_SET_INDEX = 3;
const int MESH_SKINNED_BONES_DESCRIPTOR_SET_BINDING = 0;

namespace yoyo
{
    const std::vector<VkVertexInputAttributeDescription>& VertexAttributeDescriptions();
    const std::vector<VkVertexInputBindingDescription>& VertexBindingDescriptions();

    std::vector<VkVertexInputBindingDescription> GenerateVertexBindingDescriptions(const std::vector<ShaderInput>& inputs, VkVertexInputRate input_rate);
    std::vector<VkVertexInputAttributeDescription> GenerateVertexAttributeDescriptions(const std::vector<ShaderInput>& inputs);

    class VulkanStaticMesh : public StaticMesh
    {
    public:
        VulkanStaticMesh() = default;
        virtual ~VulkanStaticMesh() = default;

        virtual void Bind(void* render_context) override;
        virtual void Unbind() override;

        virtual void UploadMeshData(bool free_host_memory = false) override;
    public:
        AllocatedBuffer<> vertex_buffer = {};
        AllocatedBuffer<uint32_t> index_buffer = {};
    };

    class VulkanSkinnedMesh : public SkinnedMesh
    {
    public:
        VulkanSkinnedMesh() = default;
        virtual ~VulkanSkinnedMesh() = default;

        virtual void Bind(void* render_context) override;
        virtual void Unbind() override;

        virtual void UploadMeshData(bool free_host_memory = false) override;

        AllocatedBuffer<> vertex_buffer = {};
        AllocatedBuffer<uint32_t> index_buffer = {};
    private:
        std::vector<VkDescriptorSet> bones_dsets = {};
        VkDescriptorSetLayout bones_ds_layout = VK_NULL_HANDLE;

        std::vector<AllocatedBuffer<Mat4x4>> bone_buffers = {};
    };
};