#pragma once

#include "Renderer/Shader.h"

#include "VulkanStructures.h"

namespace yoyo
{
    struct VulkanShaderModule
    {
        std::vector<uint32_t> code;
        VkShaderModule module;
    };

    // Holds all the shader related state to build a render pipeline
    struct VulkanShaderEffect
    {
        VkPipelineLayout pipeline_layout;
        std::vector<VulkanDescriptorSet> set_layouts;

        struct ShaderStage
        {
            VulkanShaderModule* module;
            VkShaderStageFlagBits stage;
        };

        std::vector<ShaderStage> stages;
    };

    // A Shader Pass is the built version of shader effect
    struct VulkanShaderPass
    {
        VulkanShaderEffect* effect;
        VkPipeline pipeline;
        VkPipelineLayout layout;
    };

    // Effect Template
    class VulkanShader : public Shader
    {
    public:
        VulkanShader();
        ~VulkanShader();
    private:
        std::vector<VulkanShaderPass*> m_passes;
    };
}