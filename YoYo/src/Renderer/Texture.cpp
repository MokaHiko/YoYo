#include "Texture.h"

#include <Hurno.h>
#include "Core/Log.h"

namespace yoyo
{
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