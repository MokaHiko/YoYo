#pragma once

#include "Core/Memory.h"
#include "VulkanUtils.h"
#include "VulkanStructures.h"

namespace yoyo
{
    class Material;
    class VulkanShaderEffect;
    class VulkanShaderPass;
    class VulkanRenderer;
    class VulkanMaterialSystem
    {
    public:
        void Init(VulkanRenderer* renderer);
        void Shutdown();

        Ref<Material> CreateMaterial();

        // Create a shader pass based of the shader effect passed
        Ref<VulkanShaderPass> CreateShaderPass(VkRenderPass render_pass, Ref<VulkanShaderEffect> effect);
    private:
        // Allocator and cache for materials
		Ref<DescriptorAllocator> m_descriptor_allocator;
		Ref<DescriptorLayoutCache> m_descriptor_layout_cache;

        VkDevice m_device;
        VulkanDeletionQueue* m_deletion_queue;
    };
}