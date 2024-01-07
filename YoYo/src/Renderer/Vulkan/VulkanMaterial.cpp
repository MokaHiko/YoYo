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
};