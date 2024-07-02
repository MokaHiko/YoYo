#include "Texture.h"

#include <Hurno.h>
#include "Core/Log.h"

#include "Resource/ResourceManager.h"

namespace yoyo
{
	Texture::Texture()
		:width(0), height(0), layers(1), format(TextureFormat::Unknown), 
		m_type(TextureType::Uknown), m_sampler_type(TextureSamplerType::Linear), m_sampler_address_mode(TextureAddressMode::Repeat),
		m_dirty(TextureDirtyFlags::Clean), m_live(false){}

	Texture::~Texture(){}

    void Texture::SetTextureType(TextureType type)
	{
		m_type = type;
		m_dirty |= TextureDirtyFlags::TypeChange;
    }

    template<>
	YAPI Ref<Texture> ResourceManager::Load<Texture>(const std::string& path)
	{
		const std::string name = FileNameFromFullPath(path);

		auto& texture_cache = Cache<Texture>();
		auto texture_it = std::find_if(texture_cache.begin(), texture_cache.end(), [&](const auto& it) {
			return it.second->name == name;
		});

		if (texture_it != texture_cache.end())
		{
			return texture_it->second;
		}

		auto texture = Texture::LoadFromAsset(path.c_str());
		texture->name = name;

		if (!texture)
		{
			return nullptr;
		}

		texture_cache[texture->m_id] = texture;
		return texture;
	}

    Ref<Texture> Texture::LoadFromAsset(const char *asset_path, TextureType type)
    {
		hro::Texture hro_texture = {};
		hro::TextureInfo hro_texture_info = {};

		if (hro_texture.Load(asset_path))
		{
			hro_texture.ParseInfo(&hro_texture_info);

			Ref<Texture> texture = Texture::Create(FileNameFromFullPath(asset_path), type);

			texture->width = hro_texture_info.pixel_size[0];
			texture->height = hro_texture_info.pixel_size[1];
			texture->format = (TextureFormat)((int)hro_texture_info.format);
			texture->RawData().resize(hro_texture_info.size);

			hro_texture.Unpack(&hro_texture_info, texture->RawData().data());
			texture->RawData().shrink_to_fit();

			return texture;
		}
		else
		{
			YERROR("Failed to load texture at path %s", asset_path);
			return nullptr;
		}
	}
}