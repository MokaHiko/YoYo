#include "VulkanResourceManager.h"

#include "Platform/Platform.h"

#include "VulkanTexture.h"
#include "VulkanRenderer.h"

#include "Resource/ResourceEvent.h"

namespace yoyo
{
	VkFormat ConvertTextureFormat(TextureFormat format)
	{
		switch (format)
		{
		case(TextureFormat::RGBA8):
			return VK_FORMAT_R8G8B8A8_UNORM;
			break;
		case(TextureFormat::RGB8):
			return VK_FORMAT_R8G8B8A8_UNORM;
			break;
		default:
			return VK_FORMAT_UNDEFINED;
		}
	}

	void VulkanResourceManager::Init(VulkanRenderer* renderer)
	{
		// Grab Handles
		m_device = renderer->Device();
		m_physical_device = renderer->PhysicalDevice();
		m_physical_device_props = renderer->PhysicalDeviceProperties();
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

		// Subscribe to vulkan resource events
        EventManager::Instance()->Subscribe(MeshCreatedEvent::s_event_type, [&](Ref<Event> event){
			return false;
		});
	}

	void VulkanResourceManager::Shutdown()
	{
		// TODO: Unsubscribe to resource events

		vkDestroyCommandPool(m_device, m_upload_context.command_pool, nullptr);
		vkDestroyFence(m_device, m_upload_context.fence, nullptr);

		vmaDestroyBuffer(m_allocator, m_mesh_staging_buffer.buffer, m_mesh_staging_buffer.allocation);

		vmaDestroyAllocator(m_allocator);
	}

	bool VulkanResourceManager::UploadMesh(VulkanMesh* mesh)
	{
		mesh->vertex_buffer = CreateBuffer<Vertex>(mesh->vertices.size() * sizeof(Vertex), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		void* data;
		MapMemory(m_mesh_staging_buffer.allocation, &data);
		memcpy(data, mesh->vertices.data(), mesh->vertices.size() * sizeof(Vertex));
		UnmapMemory(m_mesh_staging_buffer.allocation);

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

	bool VulkanResourceManager::UploadTexture(VulkanTexture* texture)
	{
		// Copy to staging buffer
		AllocatedBuffer staging_buffer = CreateBuffer(texture->raw_data.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

		void* pixel_ptr;
		MapMemory(staging_buffer.allocation, &pixel_ptr);
		memcpy(pixel_ptr, texture->raw_data.data(), texture->raw_data.size());
		UnmapMemory(staging_buffer.allocation);

		// Create texture
		VkExtent3D extent;
		extent.width = texture->width;
		extent.height = texture->height;
		extent.depth = 1;

		texture->allocated_image = CreateImage(extent, ConvertTextureFormat(texture->format), VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

		// Copy data to texture via immediate mode submit
		ImmediateSubmit([&](VkCommandBuffer cmd)
			{
				// ~ Transition to transfer optimal
				VkImageSubresourceRange range = {};
				range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				range.layerCount = 1;
				range.baseArrayLayer = 0;
				range.levelCount = 1;
				range.baseMipLevel = 0;

				VkImageMemoryBarrier image_to_transfer_barrier = {};
				image_to_transfer_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				image_to_transfer_barrier.pNext = nullptr;

				image_to_transfer_barrier.image = texture->allocated_image.image;
				image_to_transfer_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				image_to_transfer_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

				image_to_transfer_barrier.srcAccessMask = 0;
				image_to_transfer_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				image_to_transfer_barrier.subresourceRange = range;

				vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_to_transfer_barrier);

				// ~ Copy to from staging to texture
				VkBufferImageCopy region = {};
				region.bufferImageHeight = 0;
				region.bufferRowLength = 0;
				region.bufferOffset = 0;

				region.imageExtent = extent;
				region.imageOffset = { 0 };
				region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				region.imageSubresource.baseArrayLayer = 0;
				region.imageSubresource.layerCount = 1;
				region.imageSubresource.mipLevel = 0;

				vkCmdCopyBufferToImage(cmd, staging_buffer.buffer, texture->allocated_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

				// ~ Transition to from shader read only 
				VkImageMemoryBarrier image_to_shader_barrier = image_to_transfer_barrier;
				image_to_shader_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				image_to_shader_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				image_to_shader_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				image_to_shader_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_to_shader_barrier); });

		// Create image view
		VkImageViewCreateInfo image_view_info = vkinit::ImageViewCreateInfo(texture->allocated_image.image, ConvertTextureFormat(texture->format), VK_IMAGE_ASPECT_COLOR_BIT);
		VK_CHECK(vkCreateImageView(m_device, &image_view_info, nullptr, &texture->image_view));

		vmaDestroyBuffer(m_allocator, staging_buffer.buffer, staging_buffer.allocation);
		m_deletion_queue->Push([=](){
				vkDestroyImageView(m_device, texture->image_view, nullptr);
				vmaDestroyImage(m_allocator, texture->allocated_image.image, texture->allocated_image.allocation);
		});

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

	Ref<VulkanShaderModule> VulkanResourceManager::CreateShaderModule(const std::string& shader_path)
	{
		// TODO: cache shader module code
		Ref<VulkanShaderModule> shader_module = CreateRef<VulkanShaderModule>();
		shader_module->source_path = shader_path.substr(shader_path.find_last_of("/\\") + 1);

		char* buffer;
		size_t buffer_size;
		if (Platform::FileRead(shader_path.c_str(), &buffer, &buffer_size))
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

	AllocatedImage VulkanResourceManager::CreateImage(VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, bool mipmapped)
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

	const size_t VulkanResourceManager::PadToUniformBufferSize(size_t original_size)
	{
		size_t min_ubo_allignment = m_physical_device_props.limits.minUniformBufferOffsetAlignment;
		size_t aligned_size = original_size;

		if (min_ubo_allignment > 0)
		{
			aligned_size = (aligned_size + min_ubo_allignment - 1) & ~(min_ubo_allignment - 1);
		}
		return aligned_size;
	}

	const size_t VulkanResourceManager::PadToStorageBufferSize(size_t original_size)
	{
		size_t min_ubo_allignment = m_physical_device_props.limits.minStorageBufferOffsetAlignment;
		size_t aligned_size = original_size;

		if (min_ubo_allignment > 0)
		{
			aligned_size = (aligned_size + min_ubo_allignment - 1) & ~(min_ubo_allignment - 1);
		}
		return aligned_size;
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