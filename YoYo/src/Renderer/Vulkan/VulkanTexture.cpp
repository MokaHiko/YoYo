#include "VulkanTexture.h"

namespace yoyo
{
	Ref<Texture> Texture::Create()
	{
		return CreateRef<VulkanTexture>();
	}
}