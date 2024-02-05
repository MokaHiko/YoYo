#pragma once

#include "Core/Memory.h"
#include "VulkanUtils.h"
#include "VulkanStructures.h"

namespace yoyo
{
    class Material;
    class VulkanMaterial;
    class VulkanShaderEffect;
    class VulkanShaderPass;
    class VulkanRenderer;
    class VulkanShader;
    class VulkanResourceManager;
    class VulkanMaterialSystem
    {
    public:
        void Init(VulkanRenderer* renderer);
        void Shutdown();

        Ref<Material> CreateMaterial(Ref<VulkanShader> shader);

        bool RegisterMaterial(Ref<VulkanMaterial> material);

        // Create a shader pass based of the shader effect passed
        Ref<VulkanShaderPass> CreateShaderPass(VkRenderPass render_pass, Ref<VulkanShaderEffect> effect);

        // Called to update material with dirty flag set
        void UpdateMaterial(Ref<VulkanMaterial> material); 
    private:
        // Allocator and cache for materials
		Ref<DescriptorAllocator> m_descriptor_allocator;
		Ref<DescriptorLayoutCache> m_descriptor_layout_cache;
    private:
        VkSampler m_linear_sampler;

        VkDevice m_device;
        VulkanDeletionQueue* m_deletion_queue;
    };
}