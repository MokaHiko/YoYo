#include "VulkanMaterial.h"

#include "Core/Log.h"

namespace yoyo
{
    VulkanMaterial::VulkanMaterial()
    {

    }

    VulkanMaterial::~VulkanMaterial()
    {

    }

    void VulkanMaterial::Bind(void* render_context, MeshPassType mesh_pass_type)
    {
        const VulkanRenderContext* ctx = static_cast<VulkanRenderContext*>(render_context);

        // Bind main texture
        vkCmdBindDescriptorSets(ctx->cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->shader_passes[mesh_pass_type]->layout, MATERIAL_MAIN_TEXTURE_DESCRIPTOR_SET_INDEX, 1, &descriptors[mesh_pass_type][MATERIAL_MAIN_TEXTURE_DESCRIPTOR_SET_INDEX].set, 0, nullptr);

        // Bind material descriptor properties (floats, textures, etc)
        vkCmdBindDescriptorSets(ctx->cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->shader_passes[mesh_pass_type]->layout, MATERIAL_PROPERTIES_DESCRIPTOR_SET_INDEX, 1, &descriptors[mesh_pass_type][MATERIAL_PROPERTIES_DESCRIPTOR_SET_INDEX].set, 0, nullptr);
    }
};