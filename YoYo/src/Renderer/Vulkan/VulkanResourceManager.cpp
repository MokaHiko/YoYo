#include "VulkanResourceManager.h"

#include "Platform/Platform.h"
#include "VulkanRenderer.h"

namespace yoyo
{
	void VulkanResourceManager::Init(VulkanRenderer* renderer)
	{
		// Grab Handles
		m_device = renderer->Device();
		m_physical_device = renderer->PhysicalDevice();
		m_instance = renderer->Instance();

		m_queues = &renderer->Queues();
		m_deletion_queue = &renderer->DeletionQueue();

		// Init vmaAllocator
		VmaAllocatorCreateInfo allocator_info = {};
		allocator_info.physicalDevice = m_physical_device;
		allocator_info.device = m_device;
		allocator_info.instance = m_instance;

		VK_CHECK(vmaCreateAllocator(&allocator_info, &m_allocator));

		// Create immediate submit context
		{
			VkCommandPoolCreateInfo graphics_cp_info = vkinit::CommandPoolCreateInfo(renderer->Queues().graphics.index, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

			vkCreateCommandPool(m_device, &graphics_cp_info, nullptr, &m_upload_context.command_pool);
			VkCommandBufferAllocateInfo alloc_info = vkinit::CommandBufferAllocInfo(m_upload_context.command_pool);
			vkAllocateCommandBuffers(m_device, &alloc_info, &m_upload_context.command_buffer);

			VkFenceCreateInfo fence_info = {};
			fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fence_info.flags = 0;

			VK_CHECK(vkCreateFence(m_device, &fence_info, nullptr, &m_upload_context.fence));
		}

		// Init staging buffers
		{
			m_mesh_staging_buffer = CreateBuffer(sizeof(Vertex) * MAX_MESH_VERTICES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		}
	}

	void VulkanResourceManager::Shutdown()
	{
		vkDestroyCommandPool(m_device, m_upload_context.command_pool, nullptr);
		vkDestroyFence(m_device, m_upload_context.fence, nullptr);

		vmaDestroyBuffer(m_allocator, m_mesh_staging_buffer.buffer, m_mesh_staging_buffer.allocation);

		vmaDestroyAllocator(m_allocator);
	}

	bool VulkanResourceManager::UploadMesh(Ref<VulkanMesh> mesh)
	{
		mesh->vertex_buffer = CreateBuffer<Vertex>(mesh->vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		void* data;
		vmaMapMemory(m_allocator, m_mesh_staging_buffer.allocation, &data);
		memcpy(data, mesh->vertices.data(), mesh->vertices.size() * sizeof(Vertex));
		vmaUnmapMemory(m_allocator, m_mesh_staging_buffer.allocation);

		ImmediateSubmit([&](VkCommandBuffer cmd) {
			VkBufferCopy region = {};
			region.dstOffset = 0;
			region.srcOffset = 0;
			region.size = mesh->vertices.size() * sizeof(Vertex);

			vkCmdCopyBuffer(cmd, m_mesh_staging_buffer.buffer, mesh->vertex_buffer.buffer, 1, &region);
			});

		if (!mesh->indices.empty())
		{
			mesh->index_buffer = CreateBuffer<uint32_t>(mesh->indices.size() * sizeof(uint32_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			void* data;
			vmaMapMemory(m_allocator, m_mesh_staging_buffer.allocation, &data);
			memcpy(data, mesh->indices.data(), mesh->indices.size() * sizeof(uint32_t));
			vmaUnmapMemory(m_allocator, m_mesh_staging_buffer.allocation);

			ImmediateSubmit([&](VkCommandBuffer cmd) {
				VkBufferCopy region = {};
				region.dstOffset = 0;
				region.srcOffset = 0;
				region.size = mesh->indices.size() * sizeof(uint32_t);

				vkCmdCopyBuffer(cmd, m_mesh_staging_buffer.buffer, mesh->index_buffer.buffer, 1, &region);
				});
		}

		return true;
	}

	void VulkanResourceManager::MapMemory(VmaAllocation allocation, void** data)
	{
        vmaMapMemory(m_allocator, allocation, data);
	}

	void VulkanResourceManager::UnmapMemory(VmaAllocation allocation)
	{
		vmaUnmapMemory(m_allocator, allocation);
	}

	Ref<VulkanShaderModule> VulkanResourceManager::CreateShaderModule(const char* shader_path)
	{
		// TODO: cache shader module code
		Ref<VulkanShaderModule> shader_module = CreateRef<VulkanShaderModule>();

		char* buffer;
		size_t buffer_size;
		if (Platform::FileRead(shader_path, &buffer, &buffer_size))
		{
			VkShaderModuleCreateInfo shader_info = {};
			shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shader_info.pCode = reinterpret_cast<uint32_t*>(buffer);
			shader_info.codeSize = buffer_size;
			
			VK_CHECK(vkCreateShaderModule(m_device, &shader_info, nullptr, &shader_module->module));

			shader_module->code.resize(buffer_size / sizeof(uint32_t));
			memcpy(shader_module->code.data(), buffer, buffer_size);
			delete buffer;
		}
		else
		{
			YERROR("Failed to create shader module!");
		}

		return shader_module;
	}

	AllocatedImage VulkanResourceManager::CreateImage(VkExtent3D extent, VkFormat format, VkImageUsageFlags usage)
	{
		AllocatedImage image = {};
		VkImageCreateInfo image_info = vkinit::ImageCreateInfo(format, extent, usage);

		VmaAllocationCreateInfo alloc_info = {};
		alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		alloc_info.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		VK_CHECK(vmaCreateImage(m_allocator, &image_info, &alloc_info, &image.image, &image.allocation, nullptr));

		m_deletion_queue->Push([&]() 
		{
			vmaDestroyImage(m_allocator, image.image, image.allocation);
		});

		return image;
	}

	void VulkanResourceManager::ImmediateSubmit(std::function<void(VkCommandBuffer)>&& fn)
	{
		VkCommandBuffer cmd = m_upload_context.command_buffer;

		VkCommandBufferBeginInfo begin_info = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		vkBeginCommandBuffer(cmd, &begin_info);

		// Execute passed commands
		fn(cmd);

		vkEndCommandBuffer(cmd);

		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.pNext = nullptr;

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &m_upload_context.command_buffer;
		vkQueueSubmit(m_queues->graphics.queue, 1, &submit_info, m_upload_context.fence);

		vkWaitForFences(m_device, 1, &m_upload_context.fence, VK_TRUE, 1000000000);
		vkResetFences(m_device, 1, &m_upload_context.fence);

		// Reset command pool
		vkResetCommandPool(m_device, m_upload_context.command_pool, 0);
	}
}