#pragma once

#include "VulkanStructures.h"

#include "Core/Memory.h"
#include "Renderer/Shader.h"

namespace yoyo
{
     // Fills descriptor array from spriv code
    void ParseDescriptorSetsFromSpirV(const void* spirv_code, size_t spirv_nbytes, VkShaderStageFlagBits stage, std::vector<VulkanDescriptorSetInformation>& set_infos);

    // Holds all the shader related state to build a render pipeline
    struct VulkanShaderEffect
    {
        std::vector<VulkanDescriptorSetInformation> set_infos;

        struct ShaderStage
        {
            Ref<VulkanShaderModule> module;
            VkShaderStageFlagBits stage;
            std::vector<ShaderInput> inputs;
        };

        VkBool32 blend_enable = false;
        VkBlendFactor src_blend_factor = VK_BLEND_FACTOR_ZERO;
        VkBlendFactor dst_blend_factor = VK_BLEND_FACTOR_ZERO;
        VkBlendOp color_blend_op = VK_BLEND_OP_ZERO_EXT;
        VkBlendFactor src_alpha_blend_factor = VK_BLEND_FACTOR_ZERO;
        VkBlendFactor dst_alpha_blend_factor = VK_BLEND_FACTOR_ZERO;
        VkBlendOp alpha_blend_op = VK_BLEND_OP_ZERO_EXT;

        VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL;
        std::vector<ShaderStage> stages;
        void PushShader(Ref<VulkanShaderModule> shader_module, VkShaderStageFlagBits stage);
    };

    // A Shader Pass is the built version of shader effect
    struct VulkanShaderPass
    {
        Ref<VulkanShaderEffect> effect;
        VkPipeline pipeline;
        VkPipelineLayout layout;
    };

    // Effect Template
    class VulkanShader : public Shader
    {
    public:
        VulkanShader();
        ~VulkanShader();
    };
}