#pragma once

#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/SkinnedMesh.h"

#include "VulkanStructures.h"
#include "VulkanResourceManager.h"

namespace yoyo
{
    const std::vector<VkVertexInputAttributeDescription> &VertexAttributeDescriptions();
    const std::vector<VkVertexInputBindingDescription> &VertexBindingDescriptions();

    std::vector<VkVertexInputBindingDescription> GenerateVertexBindingDescriptions(const std::vector<ShaderInput> &inputs, VkVertexInputRate input_rate);
    std::vector<VkVertexInputAttributeDescription> GenerateVertexAttributeDescriptions(const std::vector<ShaderInput> &inputs);

    void YAPI DispatchMeshCreatedEvent(Ref<IMesh> mesh);

    template <typename VertexType, typename IndexType>
    class VulkanMesh : public Mesh<VertexType, IndexType>
    {
    public:
        AllocatedBuffer<> vertex_buffer = {};
        AllocatedBuffer<uint32_t> index_buffer = {};
    };

    template <typename VertexType, typename IndexType>
    class VulkanStaticMesh : public VulkanMesh<VertexType, IndexType>
    {
    public:
        VulkanStaticMesh() = default;
        virtual ~VulkanStaticMesh() = default;

        virtual void Bind(void *render_context) override
        {
            const VulkanRenderContext *ctx = static_cast<VulkanRenderContext *>(render_context);
            VkDeviceSize offset = 0;

            if (!indices.empty())
            {
                vkCmdBindIndexBuffer(ctx->cmd, index_buffer.buffer, offset, VK_INDEX_TYPE_UINT32);
            }

            vkCmdBindVertexBuffers(ctx->cmd, 0, 1, &vertex_buffer.buffer, &offset);
        }

        virtual void Unbind() override {}

        virtual void UploadMeshData(bool free_host_memory = false) override
        {
            VulkanResourceManager::UploadMesh<VertexType, IndexType>(this);

            if (free_host_memory)
            {
                vertices.clear();
                indices.clear();
            }

            RemoveDirtyFlags(MeshDirtyFlags::Unuploaded | MeshDirtyFlags::IndexDataChange | MeshDirtyFlags::VertexDataChange);
        }
    };

    template <typename VertexType, typename IndexType>
    std::shared_ptr<Mesh<VertexType, IndexType>> CreateVulkanStaticMesh(const std::string &name)
    {
        auto mesh = CreateRef<VulkanStaticMesh<VertexType, IndexType>>();
        mesh->name = name;

        DispatchMeshCreatedEvent(mesh);

        return mesh;
    }

    template <typename VertexType, typename IndexType>
    struct VulkanMeshRegistrar
    {
        VulkanMeshRegistrar()
        {
            MeshFactory::Register<VertexType, IndexType>("StaticMesh", CreateVulkanStaticMesh<VertexType, IndexType>);
        }
    };

    class VulkanSkinnedMesh : public SkinnedMesh
    {
    public:
        VulkanSkinnedMesh() = default;
        virtual ~VulkanSkinnedMesh() = default;

        virtual void Bind(void *render_context) override;
        virtual void Unbind() override;

        virtual void UploadMeshData(bool free_host_memory = false) override;

        AllocatedBuffer<> vertex_buffer = {};
        AllocatedBuffer<uint32_t> index_buffer = {};
    private:
        std::vector<VkDescriptorSet> bones_dsets = {};
        VkDescriptorSetLayout bones_ds_layout = VK_NULL_HANDLE;

        std::vector<AllocatedBuffer<Mat4x4>> bone_buffers = {};
    };
}