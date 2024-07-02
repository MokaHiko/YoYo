#include "VulkanTexture.h"

#include "Core/Assert.h"

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

	std::vector<char>& Texture::RawData()
	{
		return m_raw_data;
	}

    Ref<Texture> Texture::Create(const std::string &name, TextureType type)
	{
		Ref<VulkanTexture> texture = CreateRef<VulkanTexture>();
		texture->name = name;
		texture->m_type = type;
		texture->m_dirty |= TextureDirtyFlags::Unuploaded;

		EventManager::Instance().Dispatch(CreateRef<TextureCreatedEvent>(texture));
		return texture;
	}

	VulkanTexture::VulkanTexture()
		:image_view(VK_NULL_HANDLE), sampler(VK_NULL_HANDLE), allocated_image({})
	{
	}

	VulkanTexture::~VulkanTexture()
	{
	}

	void VulkanTexture::UploadTextureData(bool free_host_memory)
	{
		YASSERT(RawData().size() > 0, "Attempting to upload texture with 0 data");
		YASSERT((width + height) > 0 , "Texture must have non zero width and/or height");

		VulkanResourceManager::UploadTexture(this);
		m_dirty &= ~TextureDirtyFlags::Unuploaded;
	}

    const bool VulkanTexture::IsInitialized() const
    {
        return (allocated_image.image != VK_NULL_HANDLE) && (image_view != VK_NULL_HANDLE) && (allocated_image.allocation != VK_NULL_HANDLE);
    }
}