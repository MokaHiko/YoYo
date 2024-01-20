#pragma once

#include "Core/Memory.h"
#include "VulkanUtils.h"
#include "VulkanStructures.h"

namespace yoyo
{
    const int MAX_MESH_VERTICES = 10000;

    class VulkanMesh;
    class VulkanTexture;
    class VulkanRenderer;

    // Manages runtime vulkan resources
    class VulkanResourceManager
    {
    public:
        VulkanResourceManager() = default;
        ~VulkanResourceManager() {};

        void Init(VulkanRenderer* renderer);
        void Shutdown();

        bool UploadMesh(Ref<VulkanMesh> mesh);
        bool UploadTexture(Ref<VulkanTexture> texture);

        void MapMemory(VmaAllocation allocation, void** data);
        void UnmapMemory(VmaAllocation allocation);

        Ref<VulkanShaderModule> CreateShaderModule(const std::string& shader_path);

        template<typename T = void>
        AllocatedBuffer<T> CreateBuffer(size_t size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage, VkMemoryPropertyFlags memory_props = 0)
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

            m_deletion_queue->Push([=]()
                {
                    vmaDestroyBuffer(m_allocator, buffer.buffer, buffer.allocation);
                });

            return buffer;
        }

        AllocatedImage CreateImage(VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);

        const size_t PadToUniformBufferSize(size_t original_size) const;
    private:
        void ImmediateSubmit(std::function<void(VkCommandBuffer)>&& fn);
    private:
        AllocatedBuffer<void> m_mesh_staging_buffer; // General purpose staging buffer for mesh uploads
    private:
        VulkanUploadContext m_upload_context;

        VkDevice m_device;
        VkPhysicalDeviceProperties m_physical_device_props;
        VkPhysicalDevice m_physical_device;
        VkInstance m_instance;

        VulkanDeletionQueue* m_deletion_queue;
        VulkanQueues* m_queues;

        VmaAllocator m_allocator;
    };
}