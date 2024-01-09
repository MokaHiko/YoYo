#pragma once

#include "Core/Memory.h"
#include "VulkanStructures.h"

namespace yoyo
{
    // Fills descriptor array from spriv code
    void ParseDescriptorSetsFromSpirV(const void* spirv_code, size_t spirv_nbytes, VkShaderStageFlagBits stage, std::vector<VulkanDescriptorSet>& descriptors);

    class Material;
    class VulkanMaterialSystem
    {
    public:
        void Init();
        void Shutdown();

        Ref<Material> CreateMaterial();
    private:
    };
}