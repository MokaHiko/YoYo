#include "VulkanUtils.h"

#include <algorithm>
namespace yoyo
{
    VkPipeline PipelineBuilder::Build(VkDevice device, VkRenderPass renderPass, bool offscreen)
    {
        VkPipelineViewportStateCreateInfo view_port_state = {};
        view_port_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

        view_port_state.viewportCount = 1;
        view_port_state.pViewports = &viewport;
        view_port_state.scissorCount = 1;
        view_port_state.pScissors = &scissor;

        VkPipelineColorBlendStateCreateInfo color_blending = {};
        color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable = VK_FALSE;
        color_blending.logicOp = VK_LOGIC_OP_COPY;
        color_blending.attachmentCount = offscreen ? 0 : 1;
        color_blending.pAttachments = &color_blend_attachment;

        VkGraphicsPipelineCreateInfo pipeline_info = {};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

        pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
        pipeline_info.pStages = shader_stages.data();
        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState = &multisampling;
        pipeline_info.pDepthStencilState = &depth_stencil;
        pipeline_info.layout = pipeline_layout;
        pipeline_info.pTessellationState = &tesselation_state;
        pipeline_info.pViewportState = &view_port_state;
        pipeline_info.pColorBlendState = &color_blending;

        pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
        pipeline_info.renderPass = renderPass;
        pipeline_info.subpass = 0;

        std::vector<VkDynamicState> dynamic_states;
        if (rasterizer.lineWidth > 1.0f) {dynamic_states.push_back(VkDynamicState::VK_DYNAMIC_STATE_LINE_WIDTH);}

        // TODO: Dynamic States
        if (!dynamic_states.empty())
        {
			VkPipelineDynamicStateCreateInfo dynamic_state_info = {};
			dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamic_state_info.pNext = nullptr;
			dynamic_state_info.pDynamicStates = dynamic_states.data();
			dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());

			pipeline_info.pDynamicState = &dynamic_state_info;
        }

        VkPipeline new_pipeline;
        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &new_pipeline) != VK_SUCCESS)
        {
            printf("Failed to create graphics pipeline!\n");
            return VK_NULL_HANDLE;
        }

        return new_pipeline;
    }

    void DescriptorAllocator::Init(VkDevice device)
    {
        m_device_h = device;
    }

    void DescriptorAllocator::CleanUp()
    {
        for (auto p : m_free_pools)
        {
            vkDestroyDescriptorPool(m_device_h, p, nullptr);
        }

        for (auto p : m_used_pools)
        {

            vkDestroyDescriptorPool(m_device_h, p, nullptr);
        }
    }

    bool DescriptorAllocator::Allocate(VkDescriptorSet *set, VkDescriptorSetLayout layout)
    {
        if (m_current_pool == VK_NULL_HANDLE)
        {
            m_current_pool = GetPool();
            m_used_pools.push_back(m_current_pool);
        }

        VkDescriptorSetAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.pNext = nullptr;

        alloc_info.descriptorPool = m_current_pool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &layout;

        VkResult result = vkAllocateDescriptorSets(m_device_h, &alloc_info, set);
        bool re_alloc = false;

        switch (result)
        {
        case VK_SUCCESS:
        {
            return true;
            break;
        }
        case VK_ERROR_FRAGMENTED_POOL:
        case VK_ERROR_OUT_OF_POOL_MEMORY:
        {
            re_alloc = true;
            break;
        }
        default:
            break;
        }

        if (re_alloc)
        {
            // Store in used pools
            m_used_pools.push_back(m_current_pool);

            // Create new pool and try to allocate again
            m_current_pool = GetPool();
            alloc_info.descriptorPool = m_current_pool;
            if (vkAllocateDescriptorSets(m_device_h, &alloc_info, set) == VK_SUCCESS)
            {
                return true;
            };
        }

        return false;
    }

    VkDescriptorPool DescriptorAllocator::GetPool()
    {
        if (m_free_pools.size() > 0)
        {
            VkDescriptorPool pool = m_free_pools.back();
            m_free_pools.pop_back();

            return pool;
        }

        return create_pool(m_device_h, m_descriptor_pool_sizes, 1000, 0);
    }

    void DescriptorAllocator::Reset()
    {
        for (auto p : m_used_pools)
        {
            vkResetDescriptorPool(m_device_h, p, 0);
            m_free_pools.push_back(p);
        }

        m_used_pools.clear();
        m_current_pool = VK_NULL_HANDLE;
    }

    VkDescriptorPool create_pool(VkDevice device, const DescriptorAllocator::PoolSizes &pool_sizes, uint32_t count, VkDescriptorPoolCreateFlags create_flags)
    {
        VkDescriptorPoolCreateInfo descriptor_pool_info = {};
        descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_pool_info.pNext = nullptr;

        descriptor_pool_info.flags = create_flags;

        std::vector<VkDescriptorPoolSize> sizes;
        sizes.reserve(pool_sizes.sizes.size());

        for (auto &size : pool_sizes.sizes)
        {
            sizes.push_back({size.first, (uint32_t)(size.second * count)});
        }

        descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(sizes.size());
        descriptor_pool_info.pPoolSizes = sizes.data();
        descriptor_pool_info.maxSets = count;

        VkDescriptorPool new_pool;
        vkCreateDescriptorPool(device, &descriptor_pool_info, nullptr, &new_pool);

        return new_pool;
    }

    void DescriptorLayoutCache::Init(VkDevice device)
    {
        m_device_h = device;
    }

    VkDescriptorSetLayout DescriptorLayoutCache::CreateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo *info)
    {
        DescriptorLayoutInfo layout_info = {};
        layout_info.bindings.reserve(info->bindingCount);

        bool is_sorted = true;

        uint32_t last_binding = -1;

        // Copy to layout_info struct to check/store in cache
        for (uint32_t i = 0; i < info->bindingCount; i++)
        {
            layout_info.bindings.push_back(info->pBindings[i]);

            if (last_binding > layout_info.bindings[i].binding)
            {
                is_sorted = false;
            }
        }

        // Sort by binding
        if (!is_sorted)
        {
            std::sort(layout_info.bindings.begin(), layout_info.bindings.end(), [](const VkDescriptorSetLayoutBinding &a, const VkDescriptorSetLayoutBinding &b)
                      { return a.binding < b.binding; });
        }

        // Find in cache
        auto it = m_cache.find(layout_info);
        if (it != m_cache.end())
        {
            return m_cache[layout_info];
        }

        // Create and store otherwise
        VkDescriptorSetLayout layout;
        vkCreateDescriptorSetLayout(m_device_h, info, nullptr, &layout);

        m_cache[layout_info] = layout;
        return m_cache[layout_info];
    }

    void DescriptorLayoutCache::Clear()
    {
        for (auto &pair : m_cache)
        {
            vkDestroyDescriptorSetLayout(m_device_h, pair.second, nullptr);
        }
    }

    size_t DescriptorLayoutCache::DescriptorLayoutInfo::Hash() const
    {
        // Hash the size
        size_t result = std::hash<size_t>()(bindings.size());

        // Has each binding
        for (const VkDescriptorSetLayoutBinding &b : bindings)
        {

            size_t binding_hash_bit = b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24;

            // Shufle the binding data and xor with main hash
            result ^= std::hash<size_t>()(binding_hash_bit);
        }

        return result;
    }

    bool DescriptorLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo &other) const
    {
        if (bindings.size() != other.bindings.size())
        {
            return false;
        }

        for (uint32_t i = 0; i < bindings.size(); i++)
        {
            if (bindings[i].binding != other.bindings[i].binding)
            {
                return false;
            }

            if (bindings[i].descriptorCount != other.bindings[i].descriptorCount)
            {
                return false;
            }

            if (bindings[i].descriptorType != other.bindings[i].descriptorType)
            {
                return false;
            }

            if (bindings[i].stageFlags != other.bindings[i].stageFlags)
            {
                return false;
            }
        }

        return true;
    }

    DescriptorBuilder &DescriptorBuilder::BindBuffer(uint32_t binding, VkDescriptorBufferInfo *buffer_info, VkDescriptorType type, VkShaderStageFlags shader_stage)
    {
        // Push binding
        VkDescriptorSetLayoutBinding layout_binding = {};
        layout_binding.binding = binding;
        layout_binding.descriptorType = type;
        layout_binding.stageFlags = shader_stage;

        layout_binding.descriptorCount = 1;

        m_bindings.push_back(layout_binding);

        if (buffer_info != nullptr)
        {
			VkWriteDescriptorSet write = {};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.pNext = nullptr;

			write.descriptorType = type;
			write.descriptorCount = 1;
			write.pBufferInfo = buffer_info;
			write.dstBinding = binding;

			m_writes.push_back(write);
        }

        return *this;
    }

    DescriptorBuilder &DescriptorBuilder::BindImageArray(uint32_t binding, VkDescriptorImageInfo *image_infos, uint32_t image_count, VkDescriptorType type, VkShaderStageFlags shader_stage)
    {
        VkDescriptorSetLayoutBinding layout_binding = {};
        layout_binding.binding = binding;
        layout_binding.descriptorType = type;
        layout_binding.stageFlags = shader_stage;

        layout_binding.pImmutableSamplers = nullptr;

        layout_binding.descriptorCount = image_count;

        m_bindings.push_back(layout_binding);

        if (image_infos != nullptr)
        {
			VkWriteDescriptorSet write = {};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.pNext = nullptr;

			write.descriptorType = type;
			write.descriptorCount = image_count;
			write.pImageInfo = image_infos;
			write.dstBinding = binding;

			m_writes.push_back(write);
        }

        return *this;
    }

    DescriptorBuilder &DescriptorBuilder::BindImage(uint32_t binding, VkDescriptorImageInfo *image_info, VkDescriptorType type, VkShaderStageFlags shader_stage)
    {
        VkDescriptorSetLayoutBinding layout_binding = {};
        layout_binding.binding = binding;
        layout_binding.descriptorType = type;
        layout_binding.stageFlags = shader_stage;

        layout_binding.pImmutableSamplers = nullptr;

        layout_binding.descriptorCount = 1;

        m_bindings.push_back(layout_binding);


        if (image_info != nullptr)
        {
			VkWriteDescriptorSet write = {};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.pNext = nullptr;

			write.descriptorType = type;
			write.descriptorCount = 1;
			write.pImageInfo = image_info;
			write.dstBinding = binding;

			m_writes.push_back(write);
        }

        return *this;
    }

    bool DescriptorBuilder::Build(VkDescriptorSet *set, VkDescriptorSetLayout *layout)
    {
        // Create layout
        VkDescriptorSetLayoutCreateInfo layout_create_info = {};
        layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_create_info.pNext = nullptr;

        layout_create_info.bindingCount = static_cast<uint32_t>(m_bindings.size());
        layout_create_info.pBindings = m_bindings.data();

        // Check if only caching layout
        if (!layout)
        {
            m_cache->CreateDescriptorSetLayout(&layout_create_info);
        }
        else
        {
            *layout = m_cache->CreateDescriptorSetLayout(&layout_create_info);
        }

        if (set != nullptr && layout != nullptr)
        {
			// Allocate descriptor
			if (!(m_allocator->Allocate(set, *layout)))
			{
                YERROR("Failed to allocate descriptor set!");
				return false;
			}
        }

		// Update descriptors
        if (set != nullptr)
        {
		    // Assign descriptor set to each write
			for (auto &write : m_writes)
			{
				write.dstSet = *set;
			}

		    vkUpdateDescriptorSets(m_allocator->m_device_h, static_cast<uint32_t>(m_writes.size()), m_writes.data(), 0, nullptr);
        }

        return true;
    }

}