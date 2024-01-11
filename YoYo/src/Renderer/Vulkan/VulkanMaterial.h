#pragma once

#include "Renderer/Material.h"
#include "VulkanStructures.h"
#include "VulkanShader.h"

namespace yoyo 
{
    class VulkanMaterial : public Material
    {
    public:
        VulkanMaterial();
        virtual ~VulkanMaterial();

        virtual void Bind(void* render_context, MeshPassType type) override;
    private:
        std::pair<MeshPassType, VulkanDescriptorSet> descriptors;
    };

}