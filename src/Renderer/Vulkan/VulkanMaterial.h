#pragma once

#include "Renderer/Material.h"
#include "VulkanShader.h"

namespace yoyo 
{
    const int MATERIAL_TEXTURE_SET_INDEX = 1;
    const int MATERIAL_MAIN_TEXTURE_DESCRIPTOR_SET_BINDING = 0;
    const int MATERIAL_SPECULAR_TEXTURE_DESCRIPTOR_SET_BINDING = 1;

    const int MATERIAL_PROPERTIES_DESCRIPTOR_SET_INDEX = 2;

    class VulkanMaterial : public Material
    {
    public:
        VulkanMaterial();
        virtual ~VulkanMaterial();

        virtual void Bind(void* render_context, MeshPassType mesh_pass_type) override;
    private:
        friend class VulkanMaterialSystem;

        std::unordered_map<MeshPassType, std::unordered_map<uint32_t, VulkanDescriptorSet>> descriptors;
        AllocatedBuffer<> m_properties_buffer = {}; // The buffer that stores the public properties used by this material
    };
}