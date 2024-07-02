#pragma once
#include <vulkan/vulkan.h>

namespace yoyo
{
    namespace vkinit
    {
        // Command infos
        VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queue_family_index, VkCommandPoolCreateFlags create_flags = 0);
        VkCommandBufferAllocateInfo CommandBufferAllocInfo(VkCommandPool cmd_pool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags usage);
        VkRenderPassBeginInfo RenderPassBeginInfo(VkFramebuffer frame_buffer, VkRenderPass render_pass, VkRect2D &render_area, VkClearValue *clear_value, uint32_t clear_value_count);

        // Pipelines configuration
        VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shaderModule, const char *entrypoint = "main");
        VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo();
        VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology);
        VkPipelineRasterizationStateCreateInfo PipelineRasterizationStateCreateInfo(VkPolygonMode polygonMode, float line_width = 1.0f);
        VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo();
        VkPipelineColorBlendAttachmentState PipelineColorBlendAttachmentState(
            VkBool32 blend_enable = false,
            VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            VkBlendOp colorBlendOp = VK_BLEND_OP_ZERO_EXT,
            VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            VkBlendOp alphaBlendOp = VK_BLEND_OP_ZERO_EXT);
        VkPipelineDepthStencilStateCreateInfo PipelineDepthStencilStateCreateInfo(bool depth_test_enable,
                                                                                  bool depth_write_enable,
                                                                                  VkCompareOp depth_compare_op,
                                                                                  VkBool32 stencil_test_enabled = VK_FALSE,
                                                                                  VkStencilOp stencil_fail_op = VK_STENCIL_OP_KEEP,
                                                                                  VkStencilOp stencil_pass_op = VK_STENCIL_OP_KEEP,
                                                                                  VkStencilOp stencil_depth_fail_op = VK_STENCIL_OP_KEEP,
                                                                                  VkCompareOp stencil_compare_op = VK_COMPARE_OP_ALWAYS,
                                                                                  uint32_t stencil_compare_mask = 0xFF,
                                                                                  uint32_t stencil_write_mask = 0xFF,
                                                                                  uint32_t stencil_reference = 1);
        VkPipelineTessellationStateCreateInfo PipelineTesselationStateCreateInfo(int patchControlPoints);
        VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo();

        // Resources
        VkImageCreateInfo ImageCreateInfo(VkFormat format, VkExtent3D extent, VkImageUsageFlags usage, uint32_t layer_count, VkImageCreateFlags flags);
        VkImageViewCreateInfo ImageViewCreateInfo(VkImage image, VkFormat format, VkImageAspectFlags aspect, uint32_t layer_count = 1, VkImageViewType = VK_IMAGE_VIEW_TYPE_2D);

        VkSamplerCreateInfo SamplerCreateInfo(VkFilter filter, VkSamplerAddressMode sampler_address_mode);
    }
}