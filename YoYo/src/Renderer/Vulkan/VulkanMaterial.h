#pragma once

#include "Renderer/Material.h"
#include "VulkanStructures.h"

namespace yoyo 
{
    class VulkanMaterial : public Material
    {
    public:
        VulkanMaterial();
        virtual ~VulkanMaterial();
    private:
        VulkanDescriptorSet descriptor_set;
    };

}