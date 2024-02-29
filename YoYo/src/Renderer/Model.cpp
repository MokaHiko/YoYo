#include "Model.h"

#include "Core/Log.h"
#include "Resource/ResourceManager.h"

#include <Hurno.h>

#include "Material.h"
#include "Math/MatrixTransform.h"

namespace yoyo
{
	template<>
	YAPI Ref<Model> ResourceManager::Load<Model>(const std::string& path)
	{
		const std::string name = FileNameFromFullPath(path);

		auto& model_cache = Cache<Model>();
		auto model_it = std::find_if(model_cache.begin(), model_cache.end(), [&](const auto& it){
			return it.second->name == name;
		});
		
		if(model_it != model_cache.end())
		{
			return model_it->second;
		}

		YTRACE("[Cache Miss]: Resource Type: Model!");

		Ref<Model> model = Model::LoadFromAsset(path.c_str());
		if (!model)
		{
			return nullptr;
		}

		// model->dirty = ModelDirtyFlags::Unuploaded;
		model_cache[model->Id()] = model;

		return model;
	}

	template<>
	YAPI void ResourceManager::Free<Model>(Ref<Model> resource)
	{
		// TODO: Free resource
	}

	Ref<Model> Model::LoadFromAsset(const char* asset_path, const std::string& name)
	{
		hro::Model hro_model = {};
		if (hro_model.Load(asset_path))
		{
			char* data = (char*)malloc(hro_model.Info().index_buffer_size + hro_model.Info().vertex_buffer_size);
			hro_model.Unpack(data);

			Ref<Model> model = Model::Create(name);
			model->name = !name.empty() ? name : FileNameFromFullPath(asset_path);
			uint64_t vertex_offset = 0;
			uint64_t index_offset = 0;

			for (int i = 0; i < hro_model.Info().mesh_count; i++)
			{
				const auto& hro_mesh = hro_model.Info().mesh_infos[i];
				Ref<Mesh> mesh = Mesh::Create(FileNameFromFullPath(asset_path) + hro_mesh.name + std::to_string(i));

				int vertex_count = 0;
				switch (hro_mesh.vertex_format)
				{
				case(hro::VertexFormat::F32_PNCV):
				{
					vertex_count = hro_mesh.vertex_buffer_size / sizeof(hro::Vertex_F32_PNCV);
				}break;
				default:
				{
					YFATAL("Uknown Mesh Vertex Format!");
				}break;
				}
				mesh->vertices.resize(vertex_count);
				memcpy(mesh->vertices.data(), data + vertex_offset, hro_mesh.vertex_buffer_size);
				vertex_offset += hro_mesh.vertex_buffer_size;

				//index format
				int index_count = 0;
				switch (hro_mesh.index_format)
				{
				case(hro::IndexFormat::UINT32):
				{
					index_count = hro_mesh.index_buffer_size / sizeof(uint32_t);
				}break;
				default:
				{
					YFATAL("Uknown Mesh Index Format!");
				}break;
				}
				mesh->indices.resize(index_count);
				memcpy(mesh->indices.data(), data + index_offset + hro_model.Info().vertex_buffer_size, hro_mesh.index_buffer_size);
				index_offset += hro_mesh.index_buffer_size;

				// TODO: Make map
				// push model matrix
				Mat4x4 model_matrix = { 1.0f };
				memcpy(&model_matrix, hro_mesh.model_matrix, sizeof(float) * 16);

				model->meshes.push_back(mesh);
				model->model_matrices.push_back(TransposeMat4x4(model_matrix));
			}

			for (int i = 0; i < hro_model.Info().material_count; i++)
			{
				const auto& hro_mat = hro_model.Info().materials[i];
				Ref<Material> material = Material::Create(ResourceManager::Instance().Load<Shader>("lit_shader"), hro_mat.name);

				material->color = yoyo::Vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
				material->SetVec4("diffuse_color", yoyo::Vec4{ hro_mat.diffuse_color[0], hro_mat.diffuse_color[1], hro_mat.diffuse_color[2], 1.0f });
				material->SetVec4("specular_color", yoyo::Vec4{ hro_mat.specular_color[0], hro_mat.specular_color[1], hro_mat.specular_color[2], 1.0f });

				if (!hro_mat.diffuse_texture_path.empty())
				{
					std::string texture_name = "assets/textures/" + FileNameFromFullPath(hro_mat.diffuse_texture_path) + ".yo";
					material->SetTexture(MaterialTextureType::MainTexture, ResourceManager::Instance().Load<Texture>(texture_name));
				}
				else
				{
					std::string texture_name = "assets/textures/" + FileNameFromFullPath("white") + ".yo";
					material->SetTexture(MaterialTextureType::MainTexture, ResourceManager::Instance().Load<Texture>(texture_name));
				}
			}

			return model;
		}
		else
		{
			YERROR("Failed to load texture at path %s", asset_path);
			return nullptr;
		}
	}

	Ref<Model> Model::Create(const std::string& name)
	{
		// TODO: Store in resource manager cache
		return CreateRef<Model>();
	}
}
