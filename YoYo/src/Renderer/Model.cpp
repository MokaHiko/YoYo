#include "Model.h"

#include <Hurno.h>

#include "Core/Log.h"
#include "Math/MatrixTransform.h"

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

		// model->dirty = ModelDirtyFlags::Unuploaded;
		model_cache[model->Id()] = model;
		return model;
	}

	template<>
	YAPI void ResourceManager::Free<Model>(Ref<Model> resource)
	{
		// TODO: Free resource
	}

	Ref<StaticMesh> LoadStaticMesh(const std::string& mesh_name, void* vertex_buffer, uint64_t vertex_buffer_size, void* index_buffer, uint64_t index_buffer_size)
	{
		Ref<StaticMesh> mesh = StaticMesh::Create(mesh_name);
		mesh->vertices.resize(vertex_buffer_size / sizeof(Vertex));
		memcpy(mesh->vertices.data(), (char*)vertex_buffer, vertex_buffer_size);

		mesh->indices.resize(index_buffer_size / sizeof(uint32_t));
		memcpy(mesh->indices.data(), (char*)index_buffer, index_buffer_size);

		return mesh;
	}

	Ref<SkinnedMesh> LoadSkinnedMesh(const std::string& mesh_name, void* bone_buffer, int mesh_bone_count, void* vertex_bone_map_buffer, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
	{
		Ref<SkinnedMesh> mesh = SkinnedMesh::Create(mesh_name);
		mesh->vertices.resize(vertices.size());

		hro::VertexBoneData* vertex_bone_map = (hro::VertexBoneData*)vertex_bone_map_buffer;
		for (int i = 0; i < mesh->vertices.size(); i++)
		{
			memcpy(&mesh->vertices[i], &vertices[i], sizeof(Vertex));
			memcpy(&mesh->vertices[i].bone_ids, &vertex_bone_map[i].ids, sizeof(int32_t) * MAX_BONES_PER_VERTEX);
			memcpy(&mesh->vertices[i].bone_weights, &vertex_bone_map[i].weights, sizeof(float) * MAX_BONES_PER_VERTEX);
		}

		// Copy indices
		mesh->indices = indices;

		mesh->joints.resize(mesh_bone_count);
		mesh->bones.resize(mesh_bone_count);

		hro::Joint* hro_joints = (hro::Joint*)bone_buffer;
		for (int i = 0; i < mesh_bone_count; i++)
		{
			// Insert joint
			SkinnedMeshJoint joint = {};
			joint.name = hro_joints[i].name_buffer;
			joint.bone_id = i;
			memcpy(&joint.inverse_bind_pose_transform, hro_joints[i].inverse_bind_pose_transform, sizeof(Mat4x4));
			joint.bind_pose_transform = InverseMat4x4(joint.inverse_bind_pose_transform);

			mesh->joints[i] = joint;

			// No Pose 
			Mat4x4 bone_space_transform = {};

			// Apply transform in bone space then return to mesh space
			Mat4x4 final_joint_transform = joint.bind_pose_transform * bone_space_transform * joint.inverse_bind_pose_transform;

			// Bind Pose Transform
			mesh->bones[i] = final_joint_transform;
		}

		return mesh;
	}

	void ProcessNodeRecursive(hro::Node* node, void* data, int level = 0)
	{
		std::string debug_string = "";

		for (int i = 0; i < level; i++)
		{
			debug_string += "\t";
		}

		debug_string += "-";
		debug_string += node->name_buffer;
		YINFO("%s", debug_string.c_str());

		level += 1;
		for (int i = 0; i < node->ChildrenCount(); i++)
		{
			ProcessNodeRecursive(node->GetChild(i, data), data, level);
		}
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

			char* vertex_buffer = (char*)hro_model.VertexBuffer(&hro_model_info, data);
			char* index_buffer = (char*)hro_model.IndexBuffer(&hro_model_info, data);

			void* global_bone_buffer = (char*)hro_model.BoneBuffer(&hro_model_info, data);
			void* global_vertex_bone_map_buffer = (char*)hro_model.VertexBoneMapBuffer(&hro_model_info, data);

			uint64_t vertex_offset = 0;
			uint64_t index_offset = 0;

			Ref<Model> model = Model::Create(name);
			model->name = !name.empty() ? name : FileNameFromFullPath(asset_path);
			for (int i = 0; i < hro_model_info.mesh_count; i++)
			{
				// TODO: Check vertex format
				const hro::MeshInfo& hro_mesh = hro_model_info.mesh_infos[i];

				// Push model matrix
				Mat4x4 model_matrix = {};
				memcpy(&model_matrix, hro_mesh.model_matrix, sizeof(float) * 16);
				model->model_matrices.push_back(model_matrix);

				Ref<StaticMesh> mesh = LoadStaticMesh(
					FileNameFromFullPath(asset_path) + hro_mesh.name,
					vertex_buffer + vertex_offset, hro_mesh.vertex_buffer_size,
					index_buffer + index_offset, hro_mesh.index_buffer_size
				);
				vertex_offset += hro_mesh.vertex_buffer_size;
				index_offset += hro_mesh.index_buffer_size;

				if (hro_mesh.bone_count > 0)
				{
					// Load as skinned mesh
					Ref<SkinnedMesh> skinned_mesh = LoadSkinnedMesh(
						FileNameFromFullPath(asset_path) + hro_mesh.name,
						global_bone_buffer,
						hro_mesh.bone_count,
						global_vertex_bone_map_buffer,
						mesh->vertices,
						mesh->indices
					);

					// Load skeletal data
					skinned_mesh->skeletal_hierarchy = ResourceManager::Instance().Load<SkeletalHierarchy>("assets/skeletal_meshes/test.yskmesh");
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

				model->materials.push_back(material);
			}

			// Load Animations
			char* hro_animations = (char*)hro_model.AnimationsBuffer(&hro_model_info, data);

			uint32_t anim_ctr = 0;
			for (const auto& anim_info : hro_model_info.animation_infos)
			{
				YINFO("%s", anim_info.name.c_str());
				YINFO("pos channels: %d", anim_info.position_channel_count);
				YINFO("rot channels: %d", anim_info.rotation_channel_count);
				YINFO("scale channels: %d", anim_info.scale_channel_count);

				hro::Animation hro_animation = {};
				{
					uint64_t pos_channel_size = anim_info.position_channel_count * sizeof(hro::Channel<float[3]>);
					uint64_t rot_channel_size = anim_info.rotation_channel_count * sizeof(hro::Channel<float[4]>);
					uint64_t scale_channel_size = anim_info.scale_channel_count * sizeof(hro::Channel<float[3]>);

					hro_animation.position_channels.resize(anim_info.position_channel_count);
					memcpy(hro_animation.position_channels.data(), hro_animations, pos_channel_size);

					hro_animation.rotation_channels.resize(anim_info.rotation_channel_count);
					memcpy(hro_animation.rotation_channels.data(), hro_animations + pos_channel_size, rot_channel_size);

					hro_animation.scale_channels.resize(anim_info.scale_channel_count);
					memcpy(hro_animation.scale_channels.data(), hro_animations + pos_channel_size + rot_channel_size, scale_channel_size);
				}

				{
					Ref<Animation> animation = Animation::Create(anim_info.name);
					animation->ticks = anim_info.ticks;
					animation->ticks_per_second = anim_info.ticks_per_second;

					animation->position_channels.resize(hro_animation.position_channels.size());
					memcpy(animation->position_channels.data(), hro_animation.position_channels.data(), hro_animation.position_channels.size() * sizeof(hro_animation.position_channels[0]));

					animation->rotation_channels.resize(hro_animation.rotation_channels.size());
					memcpy(animation->rotation_channels.data(), hro_animation.rotation_channels.data(), hro_animation.rotation_channels.size() * sizeof(hro_animation.rotation_channels[0]));

					animation->scale_channels.resize(hro_animation.scale_channels.size());
					memcpy(animation->scale_channels.data(), hro_animation.scale_channels.data(), hro_animation.scale_channels.size() * sizeof(hro_animation.scale_channels[0]));
				}
			}

			// Clean up
			delete data;
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
