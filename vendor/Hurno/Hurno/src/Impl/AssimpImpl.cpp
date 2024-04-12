#include "AssimpImpl.h"

#include <iostream>
#include <chrono>

#include "Hurno.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>

#include "Model.h"

namespace hro
{
	bool ConvertModel(const char* path, const char* out_path, bool flip_uvs)
	{
		using namespace Assimp;

		Importer importer = {};
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
		DefaultLogger::create("", Assimp::Logger::NORMAL);
		DefaultLogger::get()->info("Assimp Initalized");

		unsigned int flags =
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_JoinIdenticalVertices |
			aiProcess_PopulateArmatureData |
			aiProcess_SortByPType |
			aiProcess_LimitBoneWeights;

		if (flip_uvs)
		{
			flags |= aiProcess_FlipUVs;
		}

		const aiScene* scene = importer.ReadFile(path, flags);

		if (scene == nullptr)
		{
			throw std::runtime_error("Failed to load: " + std::string(path));
			return false;
		}

		auto start = std::chrono::high_resolution_clock::now();

		AssimpLoader* model_loader = new AssimpLoader();
		model_loader->LoadScene(scene);

		auto end = std::chrono::high_resolution_clock::now();

		auto diff = end - start;
		std::cout << "Model loading took " << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / 1000000.0 << "ms" << std::endl;

		Model model{};
		model.mesh_infos = model_loader->mesh_infos;

		// Export meshes
		for (hro::Mesh& mesh : model_loader->meshes)
		{
			mesh.Pack();

			const std::string mesh_out_path = "assets/meshes/" + std::string(mesh.name) + ".ymesh";
			if (!mesh.Save(mesh_out_path.c_str()))
			{
				std::cout << "Failed to save : " << mesh_out_path << std::endl;
			}
		};

		// Export skeletal meshes
		if (model_loader->skeletal_mesh.Root())
		{
			hro::AssetInfo skeletal_mesh_info = {};
			skeletal_mesh_info.original_file_path = std::string(path);
			skeletal_mesh_info.compression_mode = CompressionMode::LZ4;
			skeletal_mesh_info.raw_data_size = model_loader->skeletal_mesh.RawDataSize();
			model_loader->skeletal_mesh.Pack(&skeletal_mesh_info);

			const std::string skeletal_out_path = "assets/skeletal_meshes/" + std::string(FileNameFromFullPath(path)) + ".yskmesh";
			if (model_loader->skeletal_mesh.Save(skeletal_out_path.c_str()))
			{
				model.skeletal_mesh_path = skeletal_out_path;
			}
		}

		// Export materials
		for (hro::Material& mat : model_loader->materials)
		{
			hro::MaterialInfo mat_info = {};
			mat.Pack(&mat_info, nullptr, 0);
			const std::string material_out_path = "assets/materials/" + std::string(mat.name) + ".ymaterial";
			mat.Save(material_out_path.c_str());
		}

		// Export animations
		for (hro::Animation& animation : model_loader->animations)
		{
			hro::AssetInfo info = {};
			info.original_file_path = std::string(path);

			animation.Pack(&info);
			const std::string out_path = "assets/animations/" + animation.name + ".yanimation";

			// TODO: Force pathable file name
			if (!animation.Save(out_path.c_str()))
			{
				std::cout << "Failed to save animation at: " << out_path << std::endl;
			}
			else
			{
				model.animation_paths.push_back(out_path);
			}
		}

		ModelInfo info = {};

		info.material_count = model_loader->material_count();
		info.materials = model_loader->materials;

		info.bone_mapping_size = model_loader->vertex_map_data_count() * sizeof(hro::VertexBoneData);
		info.bone_matrices_size = model_loader->bone_count() * sizeof(hro::Joint);

		info.compression_mode = CompressionMode::LZ4;
		info.original_file_path = std::string(path);

		// Compress model as asset file
		try
		{
			start = std::chrono::high_resolution_clock::now();
			uint64_t buffer_size = info.UnpackedSize();
			void* data = malloc(buffer_size);

			memcpy((char*)data, model_loader->vertex_bone_map_data(), info.bone_mapping_size);
			memcpy((char*)data + info.bone_mapping_size, model_loader->bones_data(), info.bone_matrices_size);

			model.Pack(&info, data, buffer_size);

			end = std::chrono::high_resolution_clock::now();
			diff = end - start;

			// Debug info
			{
				std::cout << "Mesh Count: " << model.mesh_infos.size() << std::endl;

				uint32_t total_vertices = 0;
				uint32_t total_indices = 0;
				uint32_t total_bones = 0;

				for (int i = 0; i < model.mesh_infos.size(); i++)
				{
					const MeshInfo& mesh_info = model.mesh_infos[i];
					if (mesh_info.vertex_format == VertexFormat::F32_PNCV)
					{
						total_vertices += mesh_info.vertex_buffer_size / sizeof(Vertex_F32_PNCV);
					}
					if (mesh_info.index_format == IndexFormat::UINT32)
					{
						total_indices += mesh_info.index_buffer_size / sizeof(uint32_t);
					}
					total_bones += mesh_info.bone_count;
				}

				std::cout << "Vertex Count: " << total_vertices << std::endl;
				std::cout << "Index Count: " << total_indices << std::endl;
				std::cout << "Bone Count: " << total_bones << std::endl;
				std::cout << "Animation Count: " << model_loader->animations.size() << std::endl;

				std::cout << "Material Count: " << info.material_count << std::endl;
				std::cout << "Model compression took " << std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count() / 1000000.0 << "ms" << std::endl;
			}

			// Clean Up
			delete model_loader;
			DefaultLogger::kill();

			return model.Save(out_path);
		}
		catch (std::exception e)
		{
			DefaultLogger::get()->info(e.what());

			// Clean Up
			delete model_loader;
			DefaultLogger::kill();

			return false;
		}
	}
}

std::string AssimpLoader::convert_material_texture(const aiMaterial* material, aiTextureType type, const aiScene* scene)
{
	if (material->GetTextureCount(type) <= 0)
	{
		return "";
	}

	aiString ai_path;
	if (material->GetTexture(type, 0, &ai_path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
	{
		const char* path = ai_path.data;
		std::filesystem::path output_path = std::filesystem::path(path).filename();
		output_path.replace_extension("yo");

		const aiTexture* embedded_texture = scene->GetEmbeddedTexture(path);

		if (embedded_texture)
		{
			uint32_t nChannels = 4;
			uint32_t buffer_size = embedded_texture->mWidth;
			if (!hro::ConvertTexture(embedded_texture->pcData, buffer_size, std::filesystem::path(path).string().c_str(), output_path.string().c_str()))
			{
				return "";
			}
		}
		else
		{
			// TODO: Relative path
			// hro::ConvertTexture(std::filesystem::path(path).string().c_str(), output_path.string().c_str());
		}

		return output_path.string();
	}

	return "";
}

