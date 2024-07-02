#pragma once

#include <vulkan/vulkan.h>
#include "Core/Log.h"

#define VK_CHECK(x)                          \
    do                                       \
    {                                        \
        VkResult err = x;                    \
        if (err)                             \
        {                                    \
            YERROR("Vulkan error: %d", err); \
            abort();                         \
        }                                    \
    } while (0)

namespace yoyo
{
    // Creates Graphics Pipeline using the builder pattern
	class PipelineBuilder
	{
	public:
		std::vector<VkPipelineShaderStageCreateInfo> shader_stages = {};
		VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
		VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		VkPipelineMultisampleStateCreateInfo multisampling = {};
		VkPipelineColorBlendAttachmentState color_blend_attachment = {};
		VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
		VkPipelineTessellationStateCreateInfo tesselation_state = {};
		VkPipelineLayout pipeline_layout = {};

		VkViewport viewport = {};
		VkRect2D scissor = {};
	public:
		VkPipeline Build(VkDevice device, VkRenderPass renderPass, bool offscreen = false);
	};

    // Handles the allocation descriptors and descriptor pools
    class DescriptorAllocator
    {
    public:
        struct PoolSizes
        {
            std::vector<std::pair<VkDescriptorType, float>> sizes =
                {
                    {VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f},
                    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f},
                    {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f},
                    {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f},
                    {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f},
                    {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f},
                    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f},
                    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f},
                    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f},
                    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f},
                    {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f}};
        };

        void Init(VkDevice device);
        void CleanUp();

        // Allocates single descriptor set
        bool Allocate(VkDescriptorSet *set, VkDescriptorSetLayout layout);

        // Gets free descripor pool if available and allocates new ones when needed
        // Pools are created with a factor of n * 1000 descriptor set sizes
        VkDescriptorPool GetPool();

        // Rests all pools and stores in to free pools vector
        void Reset();

        // Device handle
        VkDevice m_device_h;

    private:
        VkDescriptorPool m_current_pool = VK_NULL_HANDLE;

        PoolSizes m_descriptor_pool_sizes = {};
        std::vector<VkDescriptorPool> m_free_pools;
        std::vector<VkDescriptorPool> m_used_pools;
    };

    // Cache to store descriptor set layouts
    class DescriptorLayoutCache
    {
    public:
        void Init(VkDevice device);
        void Clear();

        // Info and Identifier for descriptor set layouts in cache map
        struct DescriptorLayoutInfo
        {
            // DescriptorSets are indentified by bindings
            std::vector<VkDescriptorSetLayoutBinding> bindings;

            bool operator==(const DescriptorLayoutInfo &other) const;

            size_t Hash() const;
        };

        VkDescriptorSetLayout CreateDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo *info);

    private:
        // Hash for Descriptor Set Layout
        struct DescriptorLayoutHash
        {
            std::size_t operator()(const DescriptorLayoutInfo &k) const
            {
                return k.Hash();
            }
        };

        std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> m_cache = {};
        VkDevice m_device_h;
    };

    // Handles Allococation of descriptor pools, for a single set, set layout and its bindings
    // then binds buffer/images to descriptors
    class DescriptorBuilder
    {
    public:
        // Creates and returns builder with reference to cache and descriptor allocator
        static DescriptorBuilder Begin(DescriptorLayoutCache *layout_cache, DescriptorAllocator *_descriptor_allocator)
        {
            DescriptorBuilder builder = {};
            builder.m_cache = layout_cache;
            builder.m_allocator = _descriptor_allocator;

            return builder;
        };

        // Queues buffer to descriptor binding
        DescriptorBuilder &BindBuffer(uint32_t binding, VkDescriptorBufferInfo *buffer_info, VkDescriptorType type, VkShaderStageFlags shader_stage);

        // Queues image to descriptor binding
        DescriptorBuilder &BindImage(uint32_t binding, VkDescriptorImageInfo *image_info, VkDescriptorType type, VkShaderStageFlags shader_stage);

        // Queues image array to descriptor binding
        DescriptorBuilder &BindImageArray(uint32_t binding, VkDescriptorImageInfo *image_infos, uint32_t image_count, VkDescriptorType type, VkShaderStageFlags shader_stage);

        // Retuns trueh if both descriptor set and layout were built
        bool Build(VkDescriptorSet *set, VkDescriptorSetLayout *layout);

    private:
        DescriptorBuilder()
            :m_cache(nullptr), m_allocator(nullptr){}

        std::vector<VkWriteDescriptorSet> m_writes;
        std::vector<VkDescriptorSetLayoutBinding> m_bindings;

        DescriptorLayoutCache *m_cache;
        DescriptorAllocator *m_allocator;
    };

    // Creates descriptor pool given sizes. Should only be used by DescriptorAllocator type
    VkDescriptorPool create_pool(VkDevice device, const DescriptorAllocator::PoolSizes &pool_sizes, uint32_t count, VkDescriptorPoolCreateFlags create_flags);
}