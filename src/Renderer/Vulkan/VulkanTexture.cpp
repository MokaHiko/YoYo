#include "VulkanTexture.h"

#include "Resource/ResourceEvent.h"
#include "VulkanResourceManager.h"
#include "Texture.h"

namespace yoyo
{
	void Texture::SetSamplerType(TextureSamplerType type)
	{
		m_sampler_type = type;
		m_dirty |= TextureDirtyFlags::SamplerType;
	}

    void Texture::SetAddressMode(TextureAddressMode mode)
	{
		m_sampler_address_mode = mode;
		m_dirty |= TextureDirtyFlags::AddressMode;
	}

	Ref<Texture> Texture::Create(const std::string& name)
	{
		Ref<VulkanTexture> texture = CreateRef<VulkanTexture>();
		texture->name = name;
		texture->m_dirty |= TextureDirtyFlags::Unuploaded;

		EventManager::Instance().Dispatch(CreateRef<TextureCreatedEvent>(texture));
		return texture;
	}

	void VulkanTexture::UploadTextureData(bool free_host_memory)
	{
		VulkanResourceManager::UploadTexture(this);
		m_dirty &= ~TextureDirtyFlags::Unuploaded;
	}
}