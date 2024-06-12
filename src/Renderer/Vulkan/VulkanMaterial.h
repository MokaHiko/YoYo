#pragma once

#include "Renderer/Material.h"
#include "VulkanShader.h"

namespace yoyo 
{
    class VulkanMaterial : public Material
    {
    public:
        VulkanMaterial();
        virtual ~VulkanMaterial();

        virtual void Bind(void* render_context, MeshPassType mesh_pass_type) override;
    private:
        friend class VulkanMaterialSystem;
        std::unordered_map<MaterialTextureType, uint32_t> texture_type_to_descriptor_set;
        std::unordered_map<MeshPassType, std::vector<VulkanDescriptorSet>> descriptors;

        AllocatedBuffer<> m_properties_buffer = {}; // The buffer that stores the public properties used by this material
    };
}