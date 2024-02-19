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
        static void Init(VulkanRenderer* renderer);
        static void Shutdown();

        static bool UploadMesh(VulkanMesh* mesh);
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

            if(manage_memory)
            {
                m_deletion_queue->Push([=]() {
                        vmaDestroyBuffer(m_allocator, buffer.buffer, buffer.allocation);
                });
            }

            return buffer;
        }

        static AllocatedImage CreateImage(VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, bool manage_memory = true /*If true memory will be managed by resource manager */ , bool mipmapped = false);

        static const size_t PadToUniformBufferSize(size_t original_size);
        static const size_t PadToStorageBufferSize(size_t original_size);

        static void ImmediateSubmit(std::function<void(VkCommandBuffer)>&& fn);
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
    };
}