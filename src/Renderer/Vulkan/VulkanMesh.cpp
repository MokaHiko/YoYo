#include "VulkanMesh.h"

#include "Resource/ResourceEvent.h"
#include "VulkanResourceManager.h"

namespace yoyo
{
    const std::vector<VkVertexInputAttributeDescription> &VertexAttributeDescriptions()
    {
        static std::vector<VkVertexInputAttributeDescription> attributes = {};

        if (!attributes.empty())
        {
            return attributes;
        }

        VkVertexInputAttributeDescription position_atr = {};
        position_atr.binding = 0;
        position_atr.location = 0;
        position_atr.format = VK_FORMAT_R32G32B32_SFLOAT;
        position_atr.offset = offsetof(Vertex, position);

        attributes.push_back(position_atr);

        VkVertexInputAttributeDescription color_atr = {};
        color_atr.binding = 0;
        color_atr.location = 1;
        color_atr.format = VK_FORMAT_R32G32B32_SFLOAT;
        color_atr.offset = offsetof(Vertex, color);

        attributes.push_back(color_atr);

        VkVertexInputAttributeDescription normal_atr = {};
        normal_atr.binding = 0;
        normal_atr.location = 2;
        normal_atr.format = VK_FORMAT_R32G32B32_SFLOAT;
        normal_atr.offset = offsetof(Vertex, normal);

        attributes.push_back(normal_atr);

        VkVertexInputAttributeDescription uv_atr = {};
        uv_atr.binding = 0;
        uv_atr.location = 3;
        uv_atr.format = VK_FORMAT_R32G32_SFLOAT;
        uv_atr.offset = offsetof(Vertex, uv);

        attributes.push_back(uv_atr);

        return attributes;
    }

    const std::vector<VkVertexInputBindingDescription> &VertexBindingDescriptions()
    {
        static std::vector<VkVertexInputBindingDescription> bindings = {};

        if (!bindings.empty())
        {
            return bindings;
        }

        VkVertexInputBindingDescription vertex_binding = {};
        vertex_binding.binding = 0;
        vertex_binding.stride = sizeof(Vertex);
        vertex_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        bindings.push_back(vertex_binding);

        return bindings;
    }

    std::vector<VkVertexInputBindingDescription> GenerateVertexBindingDescriptions(const std::vector<ShaderInput> &inputs, VkVertexInputRate input_rate)
    {
        std::vector<VkVertexInputBindingDescription> bindings = {};

        VkVertexInputBindingDescription vertex_binding = {};
        vertex_binding.binding = 0;

        uint32_t stride = 0;

        for (const ShaderInput &input : inputs)
        {
            if (input.format == Format::UNDEFINED)
            {
                continue;
            }

            stride += FormatToSize(input.format);
        }

        vertex_binding.stride = stride;
        vertex_binding.inputRate = input_rate;

        bindings.push_back(vertex_binding);
        return bindings;
    }
    std::vector<VkVertexInputAttributeDescription> GenerateVertexAttributeDescriptions(const std::vector<ShaderInput> &inputs)
    {
        std::vector<VkVertexInputAttributeDescription> attributes = {};

        uint32_t offset = 0;
        for (const ShaderInput &input : inputs)
        {
            if (input.format == Format::UNDEFINED)
            {
                continue;
            }

            VkVertexInputAttributeDescription attribute = {};
            attribute.binding = 0;
            attribute.location = input.location;
            attribute.format = (VkFormat)(input.format);
            attribute.offset = offset;

            offset += FormatToSize(input.format);
            attributes.push_back(attribute);
        }

        return attributes;
    }

    Ref<SkinnedMesh> SkinnedMesh::Create(const std::string &name)
    {
        Ref<VulkanSkinnedMesh> mesh = CreateRef<VulkanSkinnedMesh>();
        mesh->name = name;

        EventManager::Instance().Dispatch(CreateRef<MeshCreatedEvent<SkinnedMesh>>(mesh));
        return mesh;
    }

    uint64_t SkinnedMesh::Hash() const
    {
        return std::hash<SkinnedMesh>()(*this);
    }

    void VulkanSkinnedMesh::Bind(void *render_context)
    {
        const VulkanRenderContext *ctx = static_cast<VulkanRenderContext *>(render_context);
        VkDeviceSize offset = 0;

        // TODO: Move to upload mesh data
        void *data;
        VulkanResourceManager::MapMemory(bone_buffers[ctx->frame].allocation, &data);
        memcpy(data, bones.data(), bones.size() * sizeof(Mat4x4));
        VulkanResourceManager::UnmapMemory(bone_buffers[ctx->frame].allocation);

        // TODO: FIX! Encapsulate
        if (ctx->mesh_pass_type == MeshPassType::Forward)
        {
            vkCmdBindDescriptorSets(ctx->cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ctx->pipeline_layout, 3, 1, &bones_dsets[ctx->frame], 0, nullptr);
        }
        else if (ctx->mesh_pass_type == MeshPassType::Shadow)
        {
            vkCmdBindDescriptorSets(ctx->cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ctx->pipeline_layout, 1, 1, &bones_dsets[ctx->frame], 0, nullptr);
        }

        if (!indices.empty())
        {
            vkCmdBindIndexBuffer(ctx->cmd, index_buffer.buffer, offset, VK_INDEX_TYPE_UINT32);
        }

        vkCmdBindVertexBuffers(ctx->cmd, 0, 1, &vertex_buffer.buffer, &offset);
    }

    void VulkanSkinnedMesh::Unbind()
    {
    }

    void VulkanSkinnedMesh::UploadMeshData(bool free_host_memory)
    {
        VulkanResourceManager::UploadMesh<VulkanSkinnedMesh>(this);

        if (free_host_memory)
        {
            vertices.clear();
            indices.clear();
        }

        // TODO: Get Renderer Flames in Flight
        bones_dsets.resize(2);
        bone_buffers.resize(2);
        for (int i = 0; i < bone_buffers.size(); i++)
        {
            size_t paddded_bone_size = VulkanResourceManager::PadToStorageBufferSize(sizeof(Mat4x4));
            bone_buffers[i] = VulkanResourceManager::CreateBuffer<Mat4x4>(paddded_bone_size * bones.size(), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

            void *data;
            VulkanResourceManager::MapMemory(bone_buffers[i].allocation, &data);
            memcpy(data, bones.data(), bones.size() * sizeof(Mat4x4));
            VulkanResourceManager::UnmapMemory(bone_buffers[i].allocation);

            VkDescriptorBufferInfo info = {};
            info.buffer = bone_buffers[i].buffer;
            info.offset = 0;
            info.range = VK_WHOLE_SIZE;

            VulkanResourceManager::AllocateDescriptor()
                .BindBuffer(0, &info, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                .Build(&bones_dsets[i], &bones_ds_layout);
        }

        RemoveDirtyFlags(MeshDirtyFlags::Unuploaded | MeshDirtyFlags::IndexDataChange | MeshDirtyFlags::VertexDataChange);
    }

    // Register default mesh types
    static VulkanMeshRegistrar<Vertex, uint32_t> VulkanMeshRegistrarInstance;
}
