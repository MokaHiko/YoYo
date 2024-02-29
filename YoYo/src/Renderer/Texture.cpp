#include "Texture.h"

#include <Hurno.h>
#include "Core/Log.h"

#include "Resource/ResourceManager.h"

namespace yoyo
{
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

	Ref<Texture> Texture::LoadFromAsset(const char* asset_path)
	{
		hro::Texture hro_texture = {};
		if (hro_texture.Load(asset_path))
		{
			Ref<Texture> texture = Texture::Create(FileNameFromFullPath(asset_path));
			texture->width = hro_texture.Info().pixel_size[0];
			texture->height = hro_texture.Info().pixel_size[1];
			texture->format = (TextureFormat)((int)hro_texture.Info().format);
			texture->raw_data.resize(hro_texture.Info().size);

			hro_texture.Unpack(texture->raw_data.data());
			texture->raw_data.shrink_to_fit();

			return texture;
		}
		else
		{
			YERROR("Failed to load texture at path %s", asset_path);
			return nullptr;
		}
	}
}