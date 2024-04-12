#include "Model.h"

#include <Hurno.h>

#include "Core/Log.h"

#include "Mesh.h"
#include "SkinnedMesh.h"
#include "Material.h"
#include "Animation.h"
#include "Resource/ResourceManager.h"

namespace yoyo
{
	template<>
	YAPI Ref<Model> ResourceManager::Load<Model>(const std::string& path)
	{
		const std::string name = FileNameFromFullPath(path);

		auto& model_cache = Cache<Model>();
		auto model_it = std::find_if(model_cache.begin(), model_cache.end(), [&](const auto& it) {
			return it.second->name == name;
		});

		if (model_it != model_cache.end())
		{
			return model_it->second;
		}

		YWARN("[Cache Miss]: Resource Type: Model!");
		Ref<Model> model = Model::LoadFromAsset(path.c_str());
		if (!model)
		{
			return nullptr;
		}

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
		hro::ModelInfo hro_model_info = {};

		if (hro_model.Load(asset_path))
		{
			hro_model.ParseInfo(&hro_model_info);

			char* data = (char*)YAllocate(hro_model_info.UnpackedSize(), yoyo::MemoryTag::Resource);
			hro_model.Unpack(&hro_model_info, data);

			void* global_bone_buffer = (char*)hro_model.BoneBuffer(&hro_model_info, data);
			void* global_vertex_bone_map_buffer = (char*)hro_model.VertexBoneMapBuffer(&hro_model_info, data);

			uint64_t vertex_offset = 0;
			uint64_t index_offset = 0;

			Ref<Model> model = Model::Create(name);
			model->name = !name.empty() ? name : FileNameFromFullPath(asset_path);

			for(const hro::MeshInfo& hro_mesh : hro_model.mesh_infos)
			{
				// TODO: Check vertex format

				// Push model matrix
				Mat4x4 model_matrix = {};
				memcpy(&model_matrix, hro_mesh.model_matrix, sizeof(float) * 16);
				model->model_matrices.push_back(model_matrix);

				Ref<StaticMesh> mesh = StaticMesh::LoadFromAsset(hro_mesh.original_file_path.c_str());

				if (hro_mesh.bone_count > 0)
				{
					// Load as skinned mesh
					Ref<SkinnedMesh> skinned_mesh = SkinnedMesh::CreateFromStaticMesh(
						mesh,
						global_bone_buffer,
						hro_mesh.bone_count,
						global_vertex_bone_map_buffer
					);

					// Load skeletal data
					skinned_mesh->skeletal_hierarchy = ResourceManager::Instance().Load<SkeletalHierarchy>(hro_model.skeletal_mesh_path);
					model->meshes.push_back(skinned_mesh);
				}
				else
				{
					model->meshes.push_back(mesh);
				}
			}

			// Load materials
			for (int i = 0; i < hro_model_info.material_count; i++)
			{
				const auto& hro_mat = hro_model_info.materials[i];
				Ref<Material> material = Material::Create(ResourceManager::Instance().Load<Shader>("lit_shader"), hro_mat.name);

				material->SetColor(yoyo::Vec4{1.0f, 1.0f, 1.0f, 1.0f});
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

				model->materials.push_back(material);
			}

			// Load Animations
			for(const std::string& path : hro_model.animation_paths)
			{
				ResourceManager::Instance().Load<Animation>(path);
			}

			// Clean up
			YFree(data);
			return model;
		}
		else
		{
			YERROR("Failed to load model at path %s", asset_path);
			return nullptr;
		}
	}

	Ref<Model> Model::Create(const std::string& name)
	{
		// TODO: Store in resource manager cache
		return CreateRef<Model>();
	}
}
