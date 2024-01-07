#include "VulkanInitializers.h"

namespace yoyo
{
    VkCommandPoolCreateInfo vkinit::CommandPoolCreateInfo(uint32_t queue_family_index, VkCommandPoolCreateFlags create_flags)
    {
        VkCommandPoolCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.pNext = nullptr;

        info.queueFamilyIndex = queue_family_index;
        info.flags = create_flags;

        return info;
    }

    VkCommandBufferAllocateInfo vkinit::CommandBufferAllocInfo(VkCommandPool cmd_pool, VkCommandBufferLevel level)
    {
        VkCommandBufferAllocateInfo alloc{};
        alloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc.pNext = nullptr;

        alloc.commandBufferCount = 1;
        alloc.commandPool = cmd_pool;
        alloc.level = level;

        return alloc;
    }

    VkCommandBufferBeginInfo vkinit::CommandBufferBeginInfo(VkCommandBufferUsageFlags usage)
    {
        VkCommandBufferBeginInfo begin = {};
        begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin.pNext = nullptr;
        begin.pInheritanceInfo = nullptr;
        begin.flags = usage;

        return begin;
    }

    VkRenderPassBeginInfo vkinit::RenderPassBeginInfo(VkFramebuffer frame_buffer, VkRenderPass render_pass, VkRect2D &render_area, VkClearValue *clear_value, uint32_t clear_value_count)
    {
        VkRenderPassBeginInfo begin = {};
        begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        begin.pNext = nullptr;

        begin.clearValueCount = clear_value_count;
        begin.pClearValues = clear_value;

        begin.framebuffer = frame_buffer;
        begin.renderPass = render_pass;
        begin.renderArea = render_area;

        return begin;
    }

    VkPipelineShaderStageCreateInfo vkinit::PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shaderModule, const char *entrypoint)
    {
        VkPipelineShaderStageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

        info.module = shaderModule;
        info.stage = stage;
        info.pName = entrypoint;

        return info;
    }

    VkPipelineVertexInputStateCreateInfo vkinit::PipelineVertexInputStateCreateInfo()
    {
        VkPipelineVertexInputStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        info.vertexAttributeDescriptionCount = 0;
        info.pVertexAttributeDescriptions = nullptr;

        info.vertexBindingDescriptionCount = 0;
        info.pVertexBindingDescriptions = nullptr;

        return info;
    }

    VkPipelineInputAssemblyStateCreateInfo vkinit::PipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology)
    {
        VkPipelineInputAssemblyStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        info.primitiveRestartEnable = VK_FALSE;
        info.topology = topology;

        return info;
    }

    VkPipelineRasterizationStateCreateInfo vkinit::PipelineRasterizationStateCreateInfo(VkPolygonMode polygonMode)
    {
        VkPipelineRasterizationStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        info.depthClampEnable = VK_FALSE;
        info.rasterizerDiscardEnable = VK_FALSE;

        info.polygonMode = polygonMode;
        info.lineWidth = 1.0f;

        info.cullMode = VK_CULL_MODE_NONE;
        info.frontFace = VK_FRONT_FACE_CLOCKWISE;

        info.depthBiasEnable = VK_FALSE;
        info.depthBiasConstantFactor = 0.0f;
        info.depthBiasClamp = 0.0f;
        info.depthBiasSlopeFactor = 0.0f;

        return info;
    }

    VkPipelineMultisampleStateCreateInfo vkinit::PipelineMultisampleStateCreateInfo()
    {
        VkPipelineMultisampleStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        info.sampleShadingEnable = VK_FALSE;
        info.minSampleShading = 1.0f;
        info.pSampleMask = nullptr;
        info.alphaToCoverageEnable = VK_FALSE;
        info.alphaToOneEnable = VK_FALSE;

        // No multisampling
        info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        return info;
    }

    VkPipelineColorBlendAttachmentState vkinit::PipelineColorBlendAttachmentState(VkBool32 blend_enable, VkBlendFactor srcColorBlendFactor, VkBlendFactor dstColorBlendFactor, VkBlendOp colorBlendOp, VkBlendFactor srcAlphaBlendFactor, VkBlendFactor dstAlphaBlendFactor, VkBlendOp alphaBlendOp)
    {
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                              VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT |
                                              VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = blend_enable;

        if (blend_enable)
        {
            colorBlendAttachment.srcColorBlendFactor = srcColorBlendFactor;
            colorBlendAttachment.dstColorBlendFactor = dstColorBlendFactor;
            colorBlendAttachment.colorBlendOp = colorBlendOp;
            colorBlendAttachment.srcAlphaBlendFactor = srcAlphaBlendFactor;
            colorBlendAttachment.dstAlphaBlendFactor = dstAlphaBlendFactor;
            colorBlendAttachment.alphaBlendOp = alphaBlendOp;
        }

        return colorBlendAttachment;
    }

    VkPipelineDepthStencilStateCreateInfo vkinit::PipelineDepthStencilStateCreateInfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp)
    {
        VkPipelineDepthStencilStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

        info.depthTestEnable = bDepthTest ? VK_TRUE : VK_FALSE;
        info.depthWriteEnable = bDepthWrite ? VK_TRUE : VK_FALSE;
        info.depthCompareOp = bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
        info.depthBoundsTestEnable = VK_FALSE;
        info.minDepthBounds = 0.0f; // Optional
        info.maxDepthBounds = 1.0f; // Optional
        info.stencilTestEnable = VK_FALSE;

        return info;
    }

    VkPipelineTessellationStateCreateInfo vkinit::PipelineTesselationStateCreateInfo(int patchControlPoints)
    {
        VkPipelineTessellationStateCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        info.pNext = nullptr;
        info.patchControlPoints = patchControlPoints;

        return info;
    }

    VkPipelineLayoutCreateInfo vkinit::PipelineLayoutCreateInfo()
    {
        VkPipelineLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        info.pNext = nullptr;

        info.flags = 0;

        info.setLayoutCount = 0;
        info.pSetLayouts = nullptr;

        info.pushConstantRangeCount = 0;
        info.pPushConstantRanges = nullptr;

        return info;
    }

    VkImageCreateInfo vkinit::ImageCreateInfo(VkFormat format, VkExtent3D extent, VkImageUsageFlags usage)
    {
        VkImageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.pNext = nullptr;

        info.imageType = VK_IMAGE_TYPE_2D;

        info.format = format;
        info.extent = extent;

        info.mipLevels = 1;
        info.arrayLayers = 1;
        info.samples = VK_SAMPLE_COUNT_1_BIT;
        info.tiling = VK_IMAGE_TILING_OPTIMAL;
        info.usage = usage;

        return info;
    }

    VkImageViewCreateInfo vkinit::ImageViewCreateInfo(VkImage image, VkFormat format, VkImageAspectFlags aspect)
    {
        VkImageViewCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;

        info.format = format;
        info.image = image;
        info.subresourceRange.aspectMask = aspect;

        // 2D image defaults
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;

        info.subresourceRange.layerCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseMipLevel = 0;

        return info;
    }

    VkSamplerCreateInfo vkinit::SamplerCreateInfo(VkFilter filter, VkSamplerAddressMode sampler_address_mode)
    {
        VkSamplerCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.pNext = nullptr;

        info.magFilter = filter;
        info.minFilter = filter;
        info.addressModeU = sampler_address_mode;
        info.addressModeV = sampler_address_mode;
        info.addressModeW = sampler_address_mode;

        return info;
    }
}