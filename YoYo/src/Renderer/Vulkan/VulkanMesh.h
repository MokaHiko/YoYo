#pragma once

#include "Renderer/Mesh.h"
#include "VulkanStructures.h"

namespace yoyo
{
    // Returns the attribute descriptions of the current vertex format
    const std::vector<VkVertexInputAttributeDescription>& VertexAttributeDescriptions();

    // Returns the binding descriptions of the current vertex format
    const std::vector<VkVertexInputBindingDescription>& VertexBindingDescriptions();

    class VulkanMesh : public Mesh
    {
    public:
        VulkanMesh();
        virtual ~VulkanMesh();

        virtual void Bind(void* render_context) override;
        virtual void Unbind() override;

        virtual void UploadMeshData(bool free_host_memory = false) override;

        AllocatedBuffer<Vertex> vertex_buffer;
        AllocatedBuffer<uint32_t> index_buffer;
    };
};