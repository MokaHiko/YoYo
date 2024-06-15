#include "VulkanResourceManager.h"

#include "Core/Assert.h"

#include "Platform/Platform.h"

#include "VulkanTexture.h"
#include "VulkanRenderer.h"

#include "Resource/ResourceEvent.h"

namespace yoyo
{
	// General purpose staging buffer for mesh uploads
    static AllocatedBuffer<> s_mesh_staging_buffer = {}; 
    static VulkanUploadContext s_upload_context = {};

    static VkDevice s_device = VK_NULL_HANDLE;
    static VkPhysicalDeviceProperties s_physical_device_props = {};
    static VkPhysicalDevice s_physical_device = VK_NULL_HANDLE;
    static VkInstance s_instance = VK_NULL_HANDLE;

    static VulkanDeletionQueue *s_deletion_queue = nullptr;
    static VulkanQueues *s_queues = nullptr;

    static VmaAllocator s_allocator = VK_NULL_HANDLE;
    static Ref<DescriptorAllocator> s_descriptor_allocator = nullptr;
    static Ref<DescriptorLayoutCache> s_descriptor_layout_cache = nullptr;

	static VkFormat ConvertTextureFormat(TextureFormat format)
	{
		switch (format)
		{
		case (TextureFormat::RGBA8):
			return VK_FORMAT_R8G8B8A8_UNORM;
			break;
		case (TextureFormat::RGB8):
			return VK_FORMAT_R8G8B8A8_UNORM;
			break;
		case (TextureFormat::R8):
			return VK_FORMAT_R8_UNORM;
			break;
		default:
			return VK_FORMAT_UNDEFINED;
		}
	}

	void VulkanResourceManager::Init(VulkanRenderer *renderer)
	{
		// Grab Handles
		s_device = renderer->Device();
		s_physical_device = renderer->PhysicalDevice();
		s_physical_device_props = renderer->PhysicalDeviceProperties();
		s_instance = renderer->Instance();

		// TODO: Create cache and allocator for resource manager
		s_descriptor_allocator = renderer->DescAllocator();
		s_descriptor_layout_cache = renderer->DescLayoutCache();

		s_queues = &renderer->Queues();
		s_deletion_queue = &renderer->DeletionQueue();

		// Init vmaAllocator
		VmaAllocatorCreateInfo allocator_info = {};
		allocator_info.physicalDevice = s_physical_device;
		allocator_info.device = s_device;
		allocator_info.instance = s_instance;

		VK_CHECK(vmaCreateAllocator(&allocator_info, &s_allocator));

		// Create immediate submit context
		{
			VkCommandPoolCreateInfo graphics_cp_info = vkinit::CommandPoolCreateInfo(renderer->Queues().graphics.index, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

			vkCreateCommandPool(s_device, &graphics_cp_info, nullptr, &s_upload_context.command_pool);
			VkCommandBufferAllocateInfo alloc_info = vkinit::CommandBufferAllocInfo(s_upload_context.command_pool);
			vkAllocateCommandBuffers(s_device, &alloc_info, &s_upload_context.command_buffer);

			VkFenceCreateInfo fence_info = {};
			fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fence_info.flags = 0;

			VK_CHECK(vkCreateFence(s_device, &fence_info, nullptr, &s_upload_context.fence));
		}

		// Init staging buffers
		{
			s_mesh_staging_buffer = CreateBuffer<>(STAGING_BUFFER_SIZE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		}

		// Subscribe to vulkan resource events
		EventManager::Instance().Subscribe(MeshCreatedEvent<Vertex>::s_event_type, [&](Ref<Event> event)
										   { return false; });
	}

	void VulkanResourceManager::Shutdown()
	{
		// TODO: Unsubscribe to resource events

		// Destroy immediate upload context
		vkDestroyCommandPool(s_device, s_upload_context.command_pool, nullptr);
		vkDestroyFence(s_device, s_upload_context.fence, nullptr);

		// Destroy allocator
		vmaDestroyAllocator(s_allocator);
	}

    AllocatedBuffer<> &VulkanResourceManager::StagingBuffer()
    {
		return s_mesh_staging_buffer;
    }

	const VmaAllocator VulkanResourceManager::Allocator()
	{
		return s_allocator;
	}

    VulkanDeletionQueue &VulkanResourceManager::DeletionQueue()
    {
		YASSERT(s_deletion_queue != nullptr);
		return *s_deletion_queue;
    }

    bool VulkanResourceManager::UploadTexture(VulkanTexture *texture)
    {
		YASSERT(texture->layers <= 1 != ((texture->GetTextureType() & TextureType::Array) == TextureType::Array),
				"Cannot have layer count greater than 1 that is not TextureType::Array or TextureType::CubeMap!");

		// Copy to staging buffer
		AllocatedBuffer<uint8_t> staging_buffer = CreateBuffer<uint8_t>(texture->raw_data.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, false);

		void *pixel_ptr;
		MapMemory(staging_buffer.allocation, &pixel_ptr);
		memcpy(pixel_ptr, texture->raw_data.data(), texture->raw_data.size());
		UnmapMemory(staging_buffer.allocation);

		YASSERT(texture->layers > 0, "Cannot have texture with 0 layers!");

		// Create texture
		VkExtent3D extent = {};
		extent.width = static_cast<uint32_t>(texture->width);
		extent.height = static_cast<uint32_t>(texture->height);
		extent.depth = 1;

		VkDeviceSize texture_size = texture->raw_data.size() / texture->layers;

		// Check if texture already allocated
		if (!texture->IsInitialized())
		{
			texture->allocated_image = CreateImage(extent, ConvertTextureFormat(texture->format), VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, texture->layers);
		}

		// Copy data to texture via immediate mode submit
		ImmediateSubmit([&](VkCommandBuffer cmd)
						{
			// ~ Transition to transfer optimal
			VkImageSubresourceRange range = {};
			range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			range.layerCount = texture->layers;
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
			std::vector<VkBufferImageCopy> copy_regions(texture->layers);
			for(int layer = 0; layer < texture->layers; layer++)
			{
				VkBufferImageCopy region = {};
				region.bufferImageHeight = 0;
				region.bufferRowLength = 0;
				region.bufferOffset = layer * texture_size;

				region.imageExtent = { extent.width, extent.height, 1 };
				region.imageOffset = { 0, 0, 0};
				region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				region.imageSubresource.baseArrayLayer = layer;
				region.imageSubresource.layerCount = 1;
				region.imageSubresource.mipLevel = 0;

				copy_regions[layer] = region;
			}
			vkCmdCopyBufferToImage(cmd, staging_buffer.buffer, texture->allocated_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(copy_regions.size()), copy_regions.data());

			// ~ Transition to from shader read only 
			VkImageMemoryBarrier image_to_shader_barrier = image_to_transfer_barrier;
			image_to_shader_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			image_to_shader_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			image_to_shader_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			image_to_shader_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_to_shader_barrier); });

		// Destroy staging buffer
		vmaDestroyBuffer(s_allocator, staging_buffer.buffer, staging_buffer.allocation);

		if (!texture->IsInitialized())
		{
			// Create image view

			VkImageViewType view_type = {};
			switch (texture->GetTextureType())
			{
			case (TextureType::Array):
			{
				view_type = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			}
			break;
			default:
				view_type = VK_IMAGE_VIEW_TYPE_2D;
				break;
			}

			VkImageViewCreateInfo image_view_info = vkinit::ImageViewCreateInfo(texture->allocated_image.image, ConvertTextureFormat(texture->format), VK_IMAGE_ASPECT_COLOR_BIT, texture->layers, view_type);
			VK_CHECK(vkCreateImageView(s_device, &image_view_info, nullptr, &texture->image_view));

			// Create Sampler
			VkSamplerCreateInfo sampler_info = vkinit::SamplerCreateInfo((VkFilter)texture->GetSamplerType(), (VkSamplerAddressMode)texture->GetAddressMode());
			VK_CHECK(vkCreateSampler(s_device, &sampler_info, nullptr, &texture->sampler));

			s_deletion_queue->Push([=]()
								   {
				vkDestroySampler(s_device, texture->sampler, nullptr);
				vkDestroyImageView(s_device, texture->image_view, nullptr); });
		}

		return true;
	}

	void VulkanResourceManager::MapMemory(VmaAllocation allocation, void **data)
	{
		vmaMapMemory(s_allocator, allocation, data);
	}

	void VulkanResourceManager::UnmapMemory(VmaAllocation allocation)
	{
		vmaUnmapMemory(s_allocator, allocation);
	}

	Ref<VulkanShaderModule> VulkanResourceManager::CreateShaderModule(const std::string &shader_path)
	{
		// TODO: cache shader module code
		Ref<VulkanShaderModule> shader_module = CreateRef<VulkanShaderModule>();
		shader_module->source_path = shader_path.substr(shader_path.find_last_of("/\\") + 1);

		char *buffer;
		size_t buffer_size;
		if (Platform::FileRead(shader_path.c_str(), &buffer, &buffer_size))
		{
			VkShaderModuleCreateInfo shader_info = {};
			shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shader_info.pCode = reinterpret_cast<uint32_t *>(buffer);
			shader_info.codeSize = buffer_size;

			VK_CHECK(vkCreateShaderModule(s_device, &shader_info, nullptr, &shader_module->module));

			shader_module->code.resize(buffer_size / sizeof(uint32_t));
			memcpy(shader_module->code.data(), buffer, buffer_size);
			delete buffer;

			s_deletion_queue->Push([=]()
								   { vkDestroyShaderModule(s_device, shader_module->module, nullptr); });
		}
		else
		{
			YERROR("Failed to create shader module!");
		}

		return shader_module;
	}

	AllocatedImage VulkanResourceManager::CreateImage(VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, uint32_t layer_count, bool manage_memory, bool mipmapped)
	{
		AllocatedImage image = {};
		VkImageCreateInfo image_info = vkinit::ImageCreateInfo(format, extent, usage, layer_count);

		VmaAllocationCreateInfo alloc_info = {};
		alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		alloc_info.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		VK_CHECK(vmaCreateImage(s_allocator, &image_info, &alloc_info, &image.image, &image.allocation, nullptr));

		if (manage_memory)
		{
			s_deletion_queue->Push([=]()
								   { vmaDestroyImage(s_allocator, image.image, image.allocation); });
		}

		return image;
	}

	const size_t VulkanResourceManager::PadToUniformBufferSize(size_t original_size)
	{
		size_t min_ubo_allignment = s_physical_device_props.limits.minUniformBufferOffsetAlignment;
		size_t aligned_size = original_size;

		if (min_ubo_allignment > 0)
		{
			aligned_size = (aligned_size + min_ubo_allignment - 1) & ~(min_ubo_allignment - 1);
		}
		return aligned_size;
	}

	const size_t VulkanResourceManager::PadToStorageBufferSize(size_t original_size)
	{
		size_t min_ubo_allignment = s_physical_device_props.limits.minStorageBufferOffsetAlignment;
		size_t aligned_size = original_size;

		if (min_ubo_allignment > 0)
		{
			aligned_size = (aligned_size + min_ubo_allignment - 1) & ~(min_ubo_allignment - 1);
		}
		return aligned_size;
	}

	void VulkanResourceManager::ImmediateSubmit(std::function<void(VkCommandBuffer)> &&fn)
	{
		VkCommandBuffer cmd = s_upload_context.command_buffer;

		VkCommandBufferBeginInfo begin_info = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		vkBeginCommandBuffer(cmd, &begin_info);

		// Execute passed commands
		fn(cmd);

		vkEndCommandBuffer(cmd);

		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.pNext = nullptr;

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &s_upload_context.command_buffer;
		vkQueueSubmit(s_queues->graphics.queue, 1, &submit_info, s_upload_context.fence);

		vkWaitForFences(s_device, 1, &s_upload_context.fence, VK_TRUE, 1000000000);
		vkResetFences(s_device, 1, &s_upload_context.fence);

		// Reset command pool
		vkResetCommandPool(s_device, s_upload_context.command_pool, 0);
	}

	DescriptorBuilder VulkanResourceManager::AllocateDescriptor()
	{
		return DescriptorBuilder::Begin(s_descriptor_layout_cache.get(), s_descriptor_allocator.get());
	}
}