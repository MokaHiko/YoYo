#pragma once

#include "Core/Memory.h"
#include "VulkanUtils.h"
#include "VulkanStructures.h"

#include "VulkanMesh.h"

namespace yoyo
{
    const int MAX_MESH_VERTICES = 100000;

    class VulkanTexture;
    class VulkanRenderer;

    class VulkanStaticMesh;

    // Manages runtime vulkan resources
    class VulkanResourceManager
    {
    public:
        static void Init(VulkanRenderer* renderer);
        static void Shutdown();

        template<typename MeshType>
        static bool UploadMesh(MeshType* mesh)
        {
            YASSERT(mesh, "Invalid mesh!");
            YASSERT(mesh->GetVertexCount() > 0, "Empty mesh!");
            YASSERT(mesh->GetVertexCount() * mesh->GetVertexSize() <= MAX_MESH_VERTICES * mesh->GetVertexSize(), "Mesh exceeds max vertex buffer size!");

            mesh->vertex_buffer = CreateBuffer<>(mesh->GetVertexCount() * mesh->GetVertexSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            void* data;
            MapMemory(m_mesh_staging_buffer.allocation, &data);
            memcpy(data, mesh->GetVertices().data(), mesh->GetVertexCount() * mesh->GetVertexSize());
            UnmapMemory(m_mesh_staging_buffer.allocation);

            ImmediateSubmit([&](VkCommandBuffer cmd) {
                VkBufferCopy region = {};
                region.dstOffset = 0;
                region.srcOffset = 0;
                region.size = mesh->GetVertexCount() * mesh->GetVertexSize();

                vkCmdCopyBuffer(cmd, m_mesh_staging_buffer.buffer, mesh->vertex_buffer.buffer, 1, &region);
            });

            if (!mesh->GetIndices().empty())
            {
                mesh->index_buffer = CreateBuffer<uint32_t>(mesh->GetIndexCount() * mesh->GetIndexSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                void* data;
                vmaMapMemory(m_allocator, m_mesh_staging_buffer.allocation, &data);
                memcpy(data, mesh->GetIndices().data(), mesh->GetIndexCount() * mesh->GetIndexSize());
                vmaUnmapMemory(m_allocator, m_mesh_staging_buffer.allocation);

                ImmediateSubmit([&](VkCommandBuffer cmd) {
                    VkBufferCopy region = {};
                    region.dstOffset = 0;
                    region.srcOffset = 0;
                    region.size = mesh->GetIndexCount() * mesh->GetIndexSize();

                    vkCmdCopyBuffer(cmd, m_mesh_staging_buffer.buffer, mesh->index_buffer.buffer, 1, &region);
                });
            }

            return true;
        }

        static bool UploadTexture(VulkanTexture* texture);

        static void MapMemory(VmaAllocation allocation, void** data);
        static void UnmapMemory(VmaAllocation allocation);

        static Ref<VulkanShaderModule> CreateShaderModule(const std::string& shader_path);

        template<typename T = void>
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

            AllocatedBuffer<T> buffer;
            VK_CHECK(vmaCreateBuffer(m_allocator, &buffer_info, &alloc_info, &buffer.buffer, &buffer.allocation, nullptr));

            if (manage_memory)
            {
                m_deletion_queue->Push([=]() {
                    vmaDestroyBuffer(m_allocator, buffer.buffer, buffer.allocation);
                    });
            }

            return buffer;
        }

        static AllocatedImage CreateImage(VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, bool manage_memory = true /*If true memory will be managed by resource manager */, bool mipmapped = false);

        static const size_t PadToUniformBufferSize(size_t original_size);
        static const size_t PadToStorageBufferSize(size_t original_size);

        static void ImmediateSubmit(std::function<void(VkCommandBuffer)>&& fn);

        static DescriptorBuilder AllocateDescriptor();
    private:
        VulkanResourceManager() = default;
        ~VulkanResourceManager() {};
    private:
        inline static AllocatedBuffer<void> m_mesh_staging_buffer; // General purpose staging buffer for mesh uploads
        inline static VulkanUploadContext m_upload_context;

        inline static VkDevice m_device;
        inline static VkPhysicalDeviceProperties m_physical_device_props;
        inline static VkPhysicalDevice m_physical_device;
        inline static VkInstance m_instance;

        inline static VulkanDeletionQueue* m_deletion_queue;
        inline static VulkanQueues* m_queues;

        inline static VmaAllocator m_allocator;
        inline static Ref<DescriptorAllocator> m_descriptor_allocator;
        inline static Ref<DescriptorLayoutCache> m_descriptor_layout_cache;
    };
}