#include "VulkanMaterial.h"

#include "Core/Log.h"
#include "Resource/ResourceEvent.h"

namespace yoyo
{
    Ref<Material> Material::Create(Ref<Shader> shader, const std::string& name)
    {
        Ref<VulkanMaterial> material = CreateRef<VulkanMaterial>();
        material->name = name;
        material->shader = shader;
        material->SetRenderMode(MaterialRenderMode::Opaque);
        material->ToggleInstanced(shader->instanced);
        material->ToggleReceiveShadows(true);
        material->m_dirty = MaterialDirtyFlags::Clean;

        EventManager::Instance().Dispatch(CreateRef<MaterialCreatedEvent>(material));
        return material;
    }

    Ref<Material> Material::Create(Ref<Material> base, const std::string& name)
    {
        Ref<VulkanMaterial> material = CreateRef<VulkanMaterial>();
        material->name = name;
        material->shader = base->shader;
        material->SetRenderMode(MaterialRenderMode::Opaque);
        material->ToggleInstanced(base->shader->instanced);
        material->ToggleReceiveShadows(true);
        material->m_dirty = MaterialDirtyFlags::Clean;

        EventManager::Instance().Dispatch(CreateRef<MaterialCreatedEvent>(material));
        return material;
    }

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
        vkCmdBindDescriptorSets(ctx->cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->shader_passes[mesh_pass_type]->layout, MATERIAL_TEXTURE_SET_INDEX, 1, &descriptors[mesh_pass_type][MATERIAL_TEXTURE_SET_INDEX].set, 0, nullptr);

        // Bind material descriptor properties (floats, textures, etc)
        vkCmdBindDescriptorSets(ctx->cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->shader_passes[mesh_pass_type]->layout, MATERIAL_PROPERTIES_DESCRIPTOR_SET_INDEX, 1, &descriptors[mesh_pass_type][MATERIAL_PROPERTIES_DESCRIPTOR_SET_INDEX].set, 0, nullptr);
    }
};