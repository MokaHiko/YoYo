#include "VulkanMaterial.h"

namespace yoyo
{
    Ref<Material> Material::Create()
    {
        return CreateRef<VulkanMaterial>();
    }

    VulkanMaterial::VulkanMaterial()
    {
    }

    VulkanMaterial::~VulkanMaterial()
    {
    }

	void VulkanMaterial::Bind(void* render_context, MeshPassType type)
	{
        const VulkanRenderContext* ctx = static_cast<VulkanRenderContext*>(render_context);

        // main texture, material data

        //vkCmdBindDescriptorSets(ctx->cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, )
	}
};