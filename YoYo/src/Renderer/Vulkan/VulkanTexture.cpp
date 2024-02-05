#include "VulkanTexture.h"

#include "Resource/ResourceEvent.h"
#include "VulkanResourceManager.h"

namespace yoyo
{
	Ref<Texture> Texture::Create(const std::string& name)
	{
		Ref<VulkanTexture> texture = CreateRef<VulkanTexture>();
		texture->m_id = name;
		texture->m_dirty |= TextureDirtyFlags::Unuploaded;

		EventManager::Instance()->Dispatch(CreateRef<TextureCreatedEvent>(texture));
		return texture;
	}

	void VulkanTexture::UploadTextureData(bool free_host_memory)
	{
		VulkanResourceManager::UploadTexture(this);
		m_dirty &= ~TextureDirtyFlags::Unuploaded;
	}
}