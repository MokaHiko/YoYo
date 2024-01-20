#include "VulkanMesh.h"

namespace yoyo
{
    const std::vector<VkVertexInputAttributeDescription>& VertexAttributeDescriptions()
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

    const std::vector<VkVertexInputBindingDescription>& VertexBindingDescriptions()
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

    Ref<Mesh> Mesh::Create()
    {
        Ref<VulkanMesh> mesh = CreateRef<VulkanMesh>();
        return mesh;
    }

    VulkanMesh::VulkanMesh()
    {

    }

    VulkanMesh::~VulkanMesh()
    {

    }

    void VulkanMesh::Bind(void* render_context) 
    {
        const VulkanRenderContext* ctx  = static_cast<VulkanRenderContext*>(render_context);
        VkDeviceSize offset = 0;

        if(!indices.empty())
        {
            vkCmdBindIndexBuffer(ctx->cmd, index_buffer.buffer, offset, VK_INDEX_TYPE_UINT32);
        }

        vkCmdBindVertexBuffers(ctx->cmd, 0, 1, &vertex_buffer.buffer, &offset);
    }

    void VulkanMesh::Unbind() 
    {
    }

} // namespace yoyo
