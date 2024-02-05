#include "Texture.h"

#include <Hurno.h>
#include "Core/Log.h"

#include "Resource/ResourceManager.h"

namespace yoyo
{
	template<>
	Ref<Texture> ResourceManager::Load<Texture>(const std::string& path)
	{
		ResourceId id = FileNameFromFullPath(path);

		auto texture_it = m_texture_cache.find(id);
		if(texture_it != m_texture_cache.end())
		{
			return texture_it->second;
		}

		auto texture = Texture::LoadFromAsset(path.c_str());
		texture->m_id = id;
		if(!texture)
		{
			return nullptr;
		}

		m_texture_cache[texture->m_id] = texture;
		return texture;
	}

	template<>
	void ResourceManager::Free<Texture>(Ref<Texture> resource)
	{

	}

    bool ResourceManager::OnTextureCreated(Ref<Texture> texture)
    {
        // TODO: Generate string uuid
        if (texture->ID().empty())
        {
        }

        // Check if shader already cached
		auto texture_it = m_texture_cache.find(texture->ID());
		if (texture_it != m_texture_cache.end())
		{
			return true;
		}

        // TODO: Check if there is space in cache

		// Cache
        m_texture_cache[texture->ID()] = texture;
        return true;
    }

	Ref<Texture> Texture::LoadFromAsset(const char* asset_path)
	{
		hro::Texture hro_texture = {};
		if(hro_texture.Load(asset_path))
		{
			Ref<Texture> texture = Texture::Create();
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