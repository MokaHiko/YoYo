#include "Hurno.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>

namespace hro
{
	bool ConvertTexture(const char* path, const char* out_path)
	{
		int width, height, n_channels;

		stbi_uc* data = stbi_load(path, &width, &height, &n_channels, STBI_rgb_alpha);

		// TODO: Support non 4 channel textures
		n_channels = 4;

		if (!data)
		{
			throw std::runtime_error("Failed to load texture!");
		}

		if (n_channels <= 0)
		{
			throw std::runtime_error("Failed to load texture!");
		}

		TextureInfo info = {};

		uint32_t texture_size = width * height * n_channels;
		switch (n_channels)
		{
		case(4):
			info.format = TextureFormat::RGBA8;
			break;
		case(3):
			info.format = TextureFormat::RGB8;
			break;
		case(1):
			info.format = TextureFormat::R8;
			break;
		}

		info.size = width * height * n_channels;
		info.pixel_size[0] = width;
		info.pixel_size[1] = height;
		info.original_file_path = path;

		hro::Texture texture {};
		texture.Pack(&info, data, texture_size);

		stbi_image_free(data);

		if ((out_path != NULL) && (out_path[0] == '\0')) 
		{
			return texture.Save("test.yo");
		}

		return texture.Save(out_path);
	}

	bool ConvertTexture(void* buffer, uint32_t buffer_size, const char* path, const char* out_path)
	{
        int width, height, nChannels;

        // Force rgba
		stbi_uc* data = stbi_load_from_memory((const stbi_uc*)buffer, buffer_size, &width, &height, &nChannels, STBI_rgb_alpha);

        if (!data)
        {
            return false;
        }

		// Texture may have been compressed
		int texture_size = width * height * 4;

        hro::TextureInfo info = {};
        info.format = hro::TextureFormat::RGBA8;
        info.compression_mode = hro::CompressionMode::LZ4;
        info.original_file_path = path;
        info.pixel_size[0] = width;
        info.pixel_size[1] = height;
        info.size = texture_size;

		Texture texture {};
		texture.Pack(&info, data, texture_size);

		stbi_image_free(data);

		if ((out_path != NULL) && (out_path[0] == '\0')) 
		{
			return texture.Save("test.yo");
		}

		return texture.Save(out_path);
	}
}