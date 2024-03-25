#pragma once

#include "Core/Memory.h"
#include "VulkanUtils.h"
#include "VulkanStructures.h"

namespace yoyo
{
    class VulkanMaterial;
    class VulkanShaderEffect;
    class VulkanShaderPass;
    class VulkanRenderer;
    class VulkanMaterialSystem
    {
    public:
        void Init(VulkanRenderer* renderer);
        void Shutdown();

        bool RegisterMaterial(Ref<VulkanMaterial> material);

        // Create a shader pass based of the shader effect passed
        Ref<VulkanShaderPass> CreateShaderPass(VkRenderPass render_pass, Ref<VulkanShaderEffect> effect, bool offscreen = false);

        // Called to update material with dirty flag set
        void UpdateMaterial(Ref<VulkanMaterial> material); 

        // Returns a handle to the default linear sampler
        VkSampler LinearSampler() const {return m_linear_sampler;}
    private:
        // Allocator and cache for materials
		Ref<DescriptorAllocator> m_descriptor_allocator;
		Ref<DescriptorLayoutCache> m_descriptor_layout_cache;
    private:
        VkSampler m_linear_sampler;
        VkSampler m_nearest_sampler;

        VkDevice m_device;
        VulkanDeletionQueue* m_deletion_queue;
    };
}