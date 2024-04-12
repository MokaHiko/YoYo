#include "Model.h"

#include <nlohmann/json.hpp>
#include <lz4.h>

void hro::Model::PackImpl(const AssetInfo* in, void* raw_data, size_t raw_data_size)
{
	type[0] = 'M';
	type[1] = 'O';
	type[2] = 'D';
	type[3] = 'L';

	version = 1;

	const ModelInfo* info = static_cast<const ModelInfo*>(in);

	nlohmann::json meta_data = {};
	meta_data["mesh_count"] = mesh_infos.size();
	meta_data["bone_mapping_size"] = info->bone_mapping_size;
	meta_data["bone_matrices_size"] = info->bone_matrices_size;
	meta_data["original_file_path"] = info->original_file_path;

	meta_data["skeletal_mesh_path"] = skeletal_mesh_path;

	meta_data["mesh_count"] = mesh_infos.size();
	meta_data["mesh_infos"] = nlohmann::json::array();
	for(const MeshInfo& mesh_info : mesh_infos)
	{
		nlohmann::json mesh_info_meta_data;
		mesh_info_meta_data["name"] = mesh_info.name;
		mesh_info_meta_data["original_file_path"] = mesh_info.original_file_path;

		mesh_info_meta_data["bone_count"] = mesh_info.bone_count;

		mesh_info_meta_data["material_id"] = mesh_info.material_id;
		mesh_info_meta_data["model_matrix"] = nlohmann::json::array();

		for (int j = 0; j < 16; j++)
		{
			mesh_info_meta_data["model_matrix"].push_back(mesh_info.model_matrix[j]);
		}

		meta_data["mesh_infos"].push_back(mesh_info_meta_data);
	}

	meta_data["material_count"] = info->material_count;
	meta_data["materials"] = nlohmann::json::array();
	for (int i = 0; i < info->material_count; i++)
	{
		const Material& material = info->materials[i];

		nlohmann::json material_meta_data;
		material_meta_data["name"] = material.name;

		const float* ambient = material.ambient_color;
		material_meta_data["ambient_color"] = { ambient[0], ambient[1], ambient[2] };

		const float* diffuse = material.diffuse_color;
		material_meta_data["diffuse_color"] = { diffuse[0], diffuse[1], diffuse[2] };

		const float* specular = material.specular_color;
		material_meta_data["specular_color"] = { specular[0], specular[1], specular[2] };

		material_meta_data["diffuse_texture_path"] = info->materials[i].diffuse_texture_path.c_str();
		material_meta_data["specular_texture_path"] = info->materials[i].specular_texture_path.c_str();
		material_meta_data["normal_texture_path"] = info->materials[i].normal_texture_path.c_str();

		meta_data["materials"].push_back(material_meta_data);
	}

	meta_data["animation_paths"] = nlohmann::json::array();
	for(const std::string path : animation_paths)
	{
		meta_data["animation_paths"].push_back(path);
	}

	int compressed_bound = LZ4_compressBound(info->UnpackedSize());
	packed_data.resize(compressed_bound);
	int compressed_size = LZ4_compress_default((const char*)raw_data, packed_data.data(), raw_data_size, compressed_bound);
	packed_data.resize(compressed_size);

	meta_data["compression_mode"] = "LZ4";
	json_meta_data = meta_data.dump();
}

void hro::Model::UnpackImpl(const AssetInfo* in, void* dst_buffer)
{
	const ModelInfo* info = static_cast<const ModelInfo*>(in);

	nlohmann::json metadata = nlohmann::json::parse(json_meta_data);
	uint32_t bone_count = metadata["bone_matrices_size"];

	if (bone_count > 0)
	{
		skeletal_mesh_path = metadata["skeletal_mesh_path"];
	}

	nlohmann::json mesh_infos_meta_data = metadata["mesh_infos"];
	for (nlohmann::json::iterator it = mesh_infos_meta_data.begin(); it != mesh_infos_meta_data.end(); it++)
	{
		// Update mesh info
		MeshInfo mesh_info = {};
		mesh_info.name = (*it)["name"];
		mesh_info.material_id = (*it)["material_id"];
		mesh_info.bone_count = (*it)["bone_count"];
		mesh_info.original_file_path = (*it)["original_file_path"];

		// TODO: Unpack better
		nlohmann::json model_matrix = (*it)["model_matrix"];
		for (int i = 0; i < 16; i++)
		{
			mesh_info.model_matrix[i] = model_matrix[i];
		}

		mesh_infos.push_back(mesh_info);
	}

	nlohmann::json animation_paths_meta_data = metadata["animation_paths"];
	for (nlohmann::json::iterator it = animation_paths_meta_data.begin(); it != animation_paths_meta_data.end(); it++)
	{
		animation_paths.push_back((*it));
	}

	if (info->compression_mode == CompressionMode::LZ4)
	{
		LZ4_decompress_safe(packed_data.data(), (char*)dst_buffer, packed_data.size(), info->UnpackedSize());
	}
	else
	{
		// NO compression
		memcpy(dst_buffer, packed_data.data(), packed_data.size());
	}
}

void* hro::Model::BoneBuffer(ModelInfo* info, void* data)
{
	assert(info != nullptr && "ModelInfo passed is null");
	assert(data != nullptr && "Invalid data given!");
	assert(!IsPacked() && "Asset must be unpacked first!");

	return (char*)data + info->bone_mapping_size;
}

void* hro::Model::VertexBoneMapBuffer(ModelInfo* info, void* data)
{
	assert(info != nullptr && "ModelInfo passed is null");
	assert(data != nullptr && "Invalid data given!");
	assert(!IsPacked() && "Asset must be unpacked first!");

	return (char*)data;
}

void hro::Model::ParseInfo(AssetInfo* out)
{
	assert(out != nullptr && "Asset info pointer is null!");
	assert(json_meta_data.c_str() != nullptr && "Meta data string is null!");

	nlohmann::json metadata = nlohmann::json::parse(json_meta_data);
	out->compression_mode = CompressionMode::LZ4;
	out->original_file_path = metadata["original_file_path"];

	ModelInfo* info = static_cast<ModelInfo*>(out);

	info->material_count = metadata["material_count"];
	nlohmann::json materials = metadata["materials"];

	info->bone_mapping_size = metadata["bone_mapping_size"];
	info->bone_matrices_size = metadata["bone_matrices_size"];

	int material_counter = 0;
	info->materials.resize(info->material_count);
	for (nlohmann::json::iterator it = materials.begin(); it != materials.end(); it++)
	{
		Material material = {};
		material.name = (*it)["name"];

		nlohmann::json ambient = (*it)["ambient_color"];
		material.ambient_color[0] = ambient[0];
		material.ambient_color[1] = ambient[1];
		material.ambient_color[2] = ambient[2];

		nlohmann::json diffuse = (*it)["diffuse_color"];
		material.diffuse_color[0] = diffuse[0];
		material.diffuse_color[1] = diffuse[1];
		material.diffuse_color[2] = diffuse[2];

		nlohmann::json specular = (*it)["specular_color"];
		material.specular_color[0] = specular[0];
		material.specular_color[1] = specular[1];
		material.specular_color[2] = specular[2];

		material.diffuse_texture_path = (*it)["diffuse_texture_path"];
		material.specular_texture_path = (*it)["specular_texture_path"];

		info->materials[material_counter++] = material;
	}
}

