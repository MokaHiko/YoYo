#include "Texture.h"

#include <nlohmann/json.hpp>
#include <lz4.h>

namespace hro
{
	TextureFormat ParseTextureFormat(const char* f)
	{
		if (strcmp(f, "RGBA8") == 0)
			return TextureFormat::RGBA8;
		else if (strcmp(f, "RGB8") == 0)
			return TextureFormat::RGB8;
		else if (strcmp(f, "R8") == 0)
			return TextureFormat::R8;
		else
			return TextureFormat::Unknown;
	}

	void Texture::PackImpl(const AssetInfo* in, void* raw_data, size_t raw_data_size)
	{
		static const char* format_look_up[] =
		{
			"Unknown",
			"R8",
			"RGB8",
			"RGBA8",
		};

		type[0] = 'T';
		type[1] = 'E';
		type[2] = 'X';

		type[3] = 'I';

		version = 1;

		const TextureInfo* info = static_cast<const TextureInfo*>(in);

		nlohmann::json meta_data = {};
		meta_data["format"] = format_look_up[(int)info->format];
		meta_data["width"] = info->pixel_size[0];
		meta_data["height"] = info->pixel_size[1];

		meta_data["size"] = info->size;
		meta_data["original_file_path"] = info->original_file_path;
		meta_data["compression_mode"] = "LZ4";

		int compressed_bound = LZ4_compressBound(info->size);
		packed_data.resize(compressed_bound);
		int compressed_size = LZ4_compress_default((const char*)raw_data, packed_data.data(), raw_data_size, compressed_bound);
		packed_data.resize(compressed_size);

		json_meta_data = meta_data.dump();
	}

	void Texture::UnpackImpl(const AssetInfo* in, void* dst_buffer)
	{
		const TextureInfo* info = static_cast<const TextureInfo*>(in);

		if (info->compression_mode == CompressionMode::LZ4)
		{
			LZ4_decompress_safe(packed_data.data(), (char*)dst_buffer, packed_data.size(), info->size);
		}
		else
		{
			// NO compression
			memcpy(dst_buffer, packed_data.data(), packed_data.size());
		}
	}

	void Texture::ParseInfo(AssetInfo* out)
	{
		assert(out != nullptr && "Asset info pointer is null!");
		assert(json_meta_data.c_str() != nullptr && "Meta data string is null!");

		nlohmann::json texture_metadata = nlohmann::json::parse(json_meta_data);

		TextureInfo* info = static_cast<TextureInfo*>(out);

		std::string format_string = texture_metadata["format"];
		info->format = ParseTextureFormat(format_string.c_str());

		info->pixel_size[0] = texture_metadata["width"];
		info->pixel_size[1] = texture_metadata["height"];
		info->size = texture_metadata["size"];
		info->original_file_path = texture_metadata["original_file_path"];

		std::string compression_string = texture_metadata["compression_mode"];
		info->compression_mode = ParseCompressionMode(compression_string.c_str());
	}

} // namespace hro