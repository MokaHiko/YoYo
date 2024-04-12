#include "Material.h"

#include <nlohmann/json.hpp>

namespace hro
{
	Material::Material()
	{

	}

	Material::~Material()
	{
	}

	void Material::ParseInfo(AssetInfo* out)
	{
		assert(out != nullptr && "Asset info pointer is null!");
		assert(json_meta_data.c_str() != nullptr && "Meta data string is null!");

		nlohmann::json metadata = nlohmann::json::parse(json_meta_data);

		MaterialInfo* info = static_cast<MaterialInfo*>(out);

		std::string format_string = metadata["format"];
		info->original_file_path = metadata["original_file_path"];

		std::string compression_string = metadata["compression_mode"];
		info->compression_mode = ParseCompressionMode(compression_string.c_str());
	}

	void Material::PackImpl(const AssetInfo* in, void* raw_data, size_t raw_data_size)
	{
		type[0] = 'M';
		type[1] = 'A';
		type[2] = 'T';

		type[3] = 'I';

		version = 1;

		// const MaterialInfo* info = static_cast<const MaterialInfo*>(in);

		nlohmann::json meta_data = {};
		meta_data["raw_data_size"] = 0;
		meta_data["original_file_path"] = "";
		meta_data["compression_mode"] = "LZ4";
		meta_data["name"] = name;

		const float* ambient = ambient_color;
		meta_data["ambient_color"] = { ambient[0], ambient[1], ambient[2] };

		const float* diffuse = diffuse_color;
		meta_data["diffuse_color"] = { diffuse[0], diffuse[1], diffuse[2] };

		const float* specular = specular_color;
		meta_data["specular_color"] = { specular[0], specular[1], specular[2] };

		meta_data["diffuse_texture_path"] = diffuse_texture_path.c_str();
		meta_data["specular_texture_path"] = specular_texture_path.c_str();
		meta_data["normal_texture_path"] = normal_texture_path.c_str();

		meta_data["compression_mode"] = "LZ4";
		json_meta_data = meta_data.dump();
	}

	void Material::UnpackImpl(const AssetInfo* in, void* dst_buffer)
	{

	}
}