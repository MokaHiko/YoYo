#pragma once

#include "Core/Memory.h"
#include "Core/Assert.h"

#include "VulkanUtils.h"
#include "VulkanStructures.h"

#include "Renderer/Mesh.h"

namespace yoyo
{
    constexpr int MAX_MESH_VERTICES = 150080;
    constexpr size_t STAGING_BUFFER_SIZE = sizeof(Vertex) * MAX_MESH_VERTICES;

    template <typename VertexType, typename IndexType>
    class VulkanMesh;

    class VulkanTexture;
    class VulkanRenderer;

    // Manages runtime vulkan resources
    class YAPI VulkanResourceManager
    {
    public:
        static void Init(VulkanRenderer *renderer);
        static void Shutdown();

        template <typename VertexType, typename IndexType>
        static bool UploadMesh(VulkanMesh<VertexType, IndexType>* mesh)
        {
            YASSERT(mesh, "Invalid mesh!");
            YASSERT(mesh->GetVertexCount() > 0, "Empty mesh!");

            size_t vertex_data_size = mesh->GetVertexCount() * mesh->GetVertexSize();
            YASSERT(vertex_data_size <= STAGING_BUFFER_SIZE, "Mesh exceeds max vertex buffer size!");

            mesh->vertex_buffer = CreateBuffer<>(vertex_data_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            void *data;
            MapMemory(StagingBuffer().allocation, &data);
            memcpy(data, mesh->GetVertices().data(), mesh->GetVertexCount() * mesh->GetVertexSize());
            UnmapMemory(StagingBuffer().allocation);

            ImmediateSubmit([&](VkCommandBuffer cmd)
                            {
                VkBufferCopy region = {};
                region.dstOffset = 0;
                region.srcOffset = 0;
                region.size = mesh->GetVertexCount() * mesh->GetVertexSize();

                vkCmdCopyBuffer(cmd, StagingBuffer().buffer, mesh->vertex_buffer.buffer, 1, &region); });

            if (!mesh->GetIndices().empty())
            {
                mesh->index_buffer = CreateBuffer<uint32_t>(mesh->GetIndexCount() * mesh->GetIndexSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                void *data;
                vmaMapMemory(Allocator(), StagingBuffer().allocation, &data);
                memcpy(data, mesh->GetIndices().data(), mesh->GetIndexCount() * mesh->GetIndexSize());
                vmaUnmapMemory(Allocator(), StagingBuffer().allocation);

                ImmediateSubmit([&](VkCommandBuffer cmd)
                                {
                    VkBufferCopy region = {};
                    region.dstOffset = 0;
                    region.srcOffset = 0;
                    region.size = mesh->GetIndexCount() * mesh->GetIndexSize();

                    vkCmdCopyBuffer(cmd, StagingBuffer().buffer, mesh->index_buffer.buffer, 1, &region); });
            }

            return true;
        }

        static bool UploadTexture(VulkanTexture *texture);

        static void MapMemory(VmaAllocation allocation, void **data);
        static void UnmapMemory(VmaAllocation allocation);

        static Ref<VulkanShaderModule> CreateShaderModule(const std::string &shader_path);

        template <typename T = void>
        static AllocatedBuffer<T> CreateBuffer(size_t size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage, VkMemoryPropertyFlags memory_props = 0, bool manage_memory = true /*If true memory will be managed by resource manager */)
        {
            VkBufferCreateInfo buffer_info = {};
            buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_info.pNext = nullptr;

            buffer_info.usage = buffer_usage;
            buffer_info.size = (VkDeviceSize)size;
            buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VmaAllocationCreateInfo alloc_info = {};
            alloc_info.usage = memory_usage;
            alloc_info.requiredFlags = memory_props;

            AllocatedBuffer<T> buffer = {};
            VK_CHECK(vmaCreateBuffer(Allocator(), &buffer_info, &alloc_info, &buffer.buffer, &buffer.allocation, nullptr));

            if (manage_memory)
            {
                DeletionQueue().Push([=]()
                                       { vmaDestroyBuffer(Allocator(), buffer.buffer, buffer.allocation); });
            }

            return buffer;
        }

        static AllocatedImage CreateImage(VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, uint32_t layer_count = 1, VkImageCreateFlags flags = 0, bool manage_memory = true /*If true memory will be managed by resource manager */, bool mipmapped = false);

        static const size_t PadToUniformBufferSize(size_t original_size);
        static const size_t PadToStorageBufferSize(size_t original_size);

        static void ImmediateSubmit(std::function<void(VkCommandBuffer)> &&fn);

        static DescriptorBuilder AllocateDescriptor();
    private:
        VulkanResourceManager() = default;
        ~VulkanResourceManager(){};

        static AllocatedBuffer<> &StagingBuffer();
        const static VmaAllocator Allocator();
        static VulkanDeletionQueue& DeletionQueue();
    };
}