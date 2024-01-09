#pragma once

#include "Core/Memory.h"
#include "VulkanStructures.h"

namespace yoyo
{
    const int MAX_MESH_VERTICES = 10000;

    class VulkanMesh;
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

	    AllocatedBuffer CreateBuffer(size_t size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage, VkMemoryPropertyFlags memory_props);
    private:
	    void ImmediateSubmit(std::function<void(VkCommandBuffer)>&& fn);
    private:
        AllocatedBuffer m_mesh_staging_buffer; // Staging buffer for mesh vertices
    private:
        VulkanUploadContext m_upload_context;

        VkDevice m_device;
        VkPhysicalDevice m_physical_device;
        VkInstance m_instance;

        VulkanDeletionQueue* m_deletion_queue;
        VulkanQueues* m_queues;

        VmaAllocator m_allocator;
    };
}