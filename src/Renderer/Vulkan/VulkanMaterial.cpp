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
        YASSERT(base, "Cannot create material from null base!");

        Ref<VulkanMaterial> material = CreateRef<VulkanMaterial>();
        material->name = name;
        material->shader = base->shader;
        material->m_dirty = MaterialDirtyFlags::Clean;

        material->SetTexture(MaterialTextureType::MainTexture, base->MainTexture());
        material->SetRenderMode(MaterialRenderMode::Opaque);
        material->ToggleInstanced(base->shader->instanced);
        material->ToggleReceiveShadows(true);

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

        // Bind material descriptors
        for(size_t i = 0; i < descriptors[mesh_pass_type].size(); i++)
        {
            const VulkanDescriptorSet& descriptor = descriptors[mesh_pass_type][i];

            if (descriptor.set == VK_NULL_HANDLE)
            {
                continue;
            }

            vkCmdBindDescriptorSets(ctx->cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->shader_passes[mesh_pass_type]->layout, i, 1, &descriptor.set, 0, nullptr);
        }
    }
};