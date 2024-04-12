#include "Animation.h"

#include <nlohmann/json.hpp>
#include <lz4.h>

namespace hro
{
	void Animation::ParseInfo(AssetInfo* out)
	{
	}

	void Animation::PackImpl(const AssetInfo* info)
	{
		type[0] = 'A';
		type[1] = 'N';
		type[2] = 'I';
		type[3] = 'M';

		version = 1;

		nlohmann::json meta_data = {};
		meta_data["raw_data_size"] = 0;
		meta_data["original_file_path"] = info->original_file_path;
		meta_data["compression_mode"] = "LZ4";

		meta_data["name"] = name;
		meta_data["ticks"] = ticks;
		meta_data["ticks_per_second"] = ticks_per_second;

		meta_data["position_channels"] = position_channels.size();
		meta_data["rotation_channels"] = rotation_channels.size();
		meta_data["scale_channels"] = scale_channels.size();

		json_meta_data = meta_data.dump();

		uint64_t position_channels_size = position_channels.size() * sizeof(Channel<float[3]>);
		uint64_t rotation_channels_size = rotation_channels.size() * sizeof(Channel<float[4]>);
		uint64_t scale_channels_size = scale_channels.size() * sizeof(Channel<float[3]>);

		uint64_t buffer_size = position_channels_size + rotation_channels_size + scale_channels_size;
		void* data = malloc(buffer_size);

		memcpy((char*)data, position_channels.data(), position_channels_size);
		memcpy((char*)data + position_channels_size, rotation_channels.data(), rotation_channels_size);
		memcpy((char*)data + position_channels_size + rotation_channels_size, scale_channels.data(), scale_channels_size);

		int compressed_bound = LZ4_compressBound(buffer_size);
		packed_data.resize(compressed_bound);
		int compressed_size = LZ4_compress_default((const char*)data, packed_data.data(), buffer_size, compressed_bound);
		packed_data.resize(compressed_size);

		free(data);
	}

	void Animation::UnpackImpl(const AssetInfo* in, void* dst_buffer)
	{
		nlohmann::json meta_data = nlohmann::json::parse(json_meta_data);

		// meta_data["original_file_path"] = info->original_file_path;
		CompressionMode compression_mode = ParseCompressionMode(std::string(meta_data["compression_mode"]).c_str());
		name = meta_data["name"];

		ticks = meta_data["ticks"];
		ticks_per_second = meta_data["ticks_per_second"];

		uint64_t position_channel_count = meta_data["position_channels"];
		uint64_t position_channel_size = meta_data["position_channels"] * sizeof(Channel<float[3]>);

		uint64_t rotation_channel_count = meta_data["rotation_channels"];
		uint64_t rotation_channel_size = meta_data["rotation_channels"] * sizeof(Channel<float[4]>);

		uint64_t scale_channel_count = meta_data["scale_channels"];
		uint64_t scale_channel_size = meta_data["scale_channels"] * sizeof(Channel<float[3]>);

		uint64_t buffer_size = position_channel_size + rotation_channel_size + scale_channel_size;

		void* data = malloc(buffer_size);
		if (compression_mode == CompressionMode::LZ4)
		{
			LZ4_decompress_safe(packed_data.data(), (char*)data, packed_data.size(), buffer_size);
		}
		else
		{
			// NO compression
			memcpy(dst_buffer, packed_data.data(), packed_data.size());
		}

		position_channels.resize(position_channel_count);
		memcpy(position_channels.data(), data, position_channel_size);

		rotation_channels.resize(position_channel_count);
		memcpy(rotation_channels.data(), (char*)data + position_channel_size, rotation_channel_size);

		scale_channels.resize(position_channel_count);
		memcpy(scale_channels.data(), (char*)data + position_channel_size + rotation_channel_size, scale_channel_size);

		free(data);
	}
}