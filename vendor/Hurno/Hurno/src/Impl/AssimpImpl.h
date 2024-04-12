#pragma once

#include <assimp/scene.h>
#include <filesystem>
#include <unordered_map>
#include <math.h>

#include "Mesh.h"
#include "Material.h"
#include "Animation.h"
#include "SkeletalMesh.h"

void print_assimp_matrix(const aiMatrix4x4& m)
{
    printf("-------------------\n");
    printf("%.2f %.2f %.2f %.2f\n", m.a1, m.a2, m.a3, m.a4);
    printf("%.2f %.2f %.2f %.2f\n", m.b1, m.b2, m.b3, m.b4);
    printf("%.2f %.2f %.2f %.2f\n", m.c1, m.c2, m.c3, m.c4);
    printf("%.2f %.2f %.2f %.2f\n", m.d1, m.d2, m.d3, m.d4);
    printf("-------------------\n");
}

class AssimpLoader
{
public:
    uint64_t mesh_count() const { return mesh_infos.size(); }

    uint64_t material_count() const { return materials.size(); }
    hro::Material* material_data() { return materials.data(); }

    hro::VertexBoneData* vertex_bone_map_data() { return vertex_to_joints.data(); }
    uint64_t vertex_map_data_count() { return vertex_to_joints.size(); }

    hro::Joint* bones_data() { return joints.data(); }
    uint64_t bone_count() { return joints.size(); }
public:
    // Meta data
    std::vector<hro::MeshInfo> mesh_infos = {};
    std::vector<hro::Material> materials = {};
    hro::SkeletalMesh skeletal_mesh;    

	std::vector<hro::Animation> animations = {};
	std::vector<hro::Mesh> meshes = {};

    uint64_t node_counter = 0;

    ~AssimpLoader() {}

    void ProcessNodeRecursive(hro::Node* node, aiNode* ai_node, const aiMatrix4x4& parent_global_matrix, const aiScene* scene)
    {
         const aiMatrix4x4 global_node_model_matrix = parent_global_matrix * ai_node->mTransformation;

        // Copy Node data

        // Check if Joint id
        auto it = node_name_to_bones.find(std::string(ai_node->mName.C_Str()));
        if(it != node_name_to_bones.end())
        {
			int node_bone_index = it->second;
			node->joint_index = node_bone_index;
        }
        else
        {
            node->joint_index = hro::INVALID_JOINT_INDEX;
        }

		// Copy name
		memcpy(node->name_buffer, &ai_node->mName.data, std::min((uint32_t)sizeof(node->name_buffer), ai_node->mName.length));

		// Transform relative to parent node
		aiMatrix4x4 transform_matrix = global_inverse_transform * ai_node->mTransformation;
		transform_matrix = transform_matrix.Transpose();
		memcpy(node->transformation, &transform_matrix, sizeof(float) * 16);

		// Call for children
		for (uint32_t i = 0; i < ai_node->mNumChildren; i++)
		{
			hro::Node* child_node = skeletal_mesh.CreateNode();
			node->AddChild(child_node);
			ProcessNodeRecursive(node->GetChild(i, skeletal_mesh.Root()), ai_node->mChildren[i], global_node_model_matrix, scene);
		}
    }

    void LoadScene(const aiScene* scene)
    {
        uint32_t mesh_count = scene->mNumMeshes;
        uint32_t material_count = scene->mNumMaterials;

        const aiMatrix4x4 identity = {};
        global_inverse_transform = scene->mRootNode->mTransformation.Inverse();

        // Materials
        ProcessMaterials(scene);

        // Node Hierarchy
        ProcessNodeRecursive(scene->mRootNode, identity, scene);

        // Animations
        for(int i = 0; i < scene->mNumAnimations; i++)
        {
            const aiAnimation* ai_animation = scene->mAnimations[i];
            hro::Animation animation = {};
            animation.name = ai_animation->mName.C_Str();
            animation.name = animation.name.substr(animation.name.find_last_of("|") + 1);
            animation.ticks = ai_animation->mDuration;
            animation.ticks_per_second = ai_animation->mTicksPerSecond;

            // Loop through nodes effected by this animation
            for(int j = 0; j < ai_animation->mNumChannels; j++)
            {
                const aiNodeAnim* ai_node_anim = ai_animation->mChannels[j];
                
                // Do not process animation nodes without bones
                const auto it = node_name_to_bones.find(std::string(ai_node_anim->mNodeName.data));
                if(it == node_name_to_bones.end())
                {
                    continue;
                }
                uint32_t bone_index = it->second;

                for(int pos_key = 0; pos_key < ai_node_anim->mNumPositionKeys; pos_key++)
                {
                    hro::Channel<float[3]> pos_chanel = {};
                    pos_chanel.bone_index = bone_index;
                    pos_chanel.time = ai_node_anim->mPositionKeys[pos_key].mTime;

                    pos_chanel.data[0] = ai_node_anim->mPositionKeys[pos_key].mValue.x;
                    pos_chanel.data[1] = ai_node_anim->mPositionKeys[pos_key].mValue.y;
                    pos_chanel.data[2] = ai_node_anim->mPositionKeys[pos_key].mValue.z;

                    animation.position_channels.push_back(pos_chanel);
                }

                for(int rot_key = 0; rot_key < ai_node_anim->mNumRotationKeys; rot_key++)
                {
                    hro::Channel<float[4]> rot_chanel = {};
                    rot_chanel.bone_index = bone_index;
                    rot_chanel.time = ai_node_anim->mRotationKeys[rot_key].mTime; 

                    rot_chanel.data[0] = ai_node_anim->mRotationKeys[rot_key].mValue.x;
                    rot_chanel.data[1] = ai_node_anim->mRotationKeys[rot_key].mValue.y;
                    rot_chanel.data[2] = ai_node_anim->mRotationKeys[rot_key].mValue.z;
                    rot_chanel.data[3] = ai_node_anim->mRotationKeys[rot_key].mValue.w;

                    animation.rotation_channels.push_back(rot_chanel);
                }

                for(int scale_key = 0; scale_key < ai_node_anim->mNumScalingKeys; scale_key++)
                {
                    hro::Channel<float[3]> scale_chanel = {};
                    scale_chanel.bone_index = bone_index;
                    scale_chanel.time = ai_node_anim->mScalingKeys[scale_key].mTime; 

                    scale_chanel.data[0] = ai_node_anim->mScalingKeys[scale_key].mValue.x;
                    scale_chanel.data[1] = ai_node_anim->mScalingKeys[scale_key].mValue.y;
                    scale_chanel.data[2] = ai_node_anim->mScalingKeys[scale_key].mValue.z;

                    animation.scale_channels.push_back(scale_chanel);
                }
            }

            // If animation does not write to channel
            if (animation.position_channels.size() + animation.scale_channels.size() + animation.rotation_channels.size() <= 0)
            {
                continue;
            }

            animations.push_back(animation);
        }

        // Skeletal mesh
        if (joints.size() > 0)
        {
			skeletal_mesh.Allocate(sizeof(hro::Node) * node_counter);
			hro::Node* root = skeletal_mesh.CreateNode();
			aiMatrix4x4 root_transform_matrix = global_inverse_transform * scene->mRootNode->mTransformation.Transpose();
			memcpy(root->transformation, &root_transform_matrix, sizeof(float) * 16);

            // Node Hierarchy w bones
            ProcessNodeRecursive(root, scene->mRootNode, identity, scene);
        }
    }
private:
    void ProcessNodeRecursive(aiNode* node, const aiMatrix4x4& parent_global_matrix, const aiScene* scene)
    {
        const aiMatrix4x4 global_node_model_matrix = parent_global_matrix * node->mTransformation;

        // Keep track of nodes
        node_counter++;

        // Load meshes
        for (uint32_t i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            ProcessMesh(mesh, global_node_model_matrix, scene);
        }

        // Call for children
        for (uint32_t i = 0; i < node->mNumChildren; i++)
        {
            ProcessNodeRecursive(node->mChildren[i], global_node_model_matrix, scene);
        }
    }

    void ProcessMesh(aiMesh* mesh, const aiMatrix4x4& global_node_model_matrix, const aiScene* scene)
    {
        hro::Mesh hro_mesh = {};
        hro_mesh.name = mesh->mName.C_Str();

        // Vertices
        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            aiVector3D position = mesh->mVertices[i];
            aiVector3D normal = {0.0f, 0.0f, 0.0f};

            if (mesh->HasNormals())
            {
                normal = mesh->mNormals[i];
            }

            hro::Vertex_F32_PNCV vertex = {};

            vertex.position[0] = position.x;
            vertex.position[1] = position.y;
            vertex.position[2] = position.z;

            vertex.normal[0] = normal.x;
            vertex.normal[1] = normal.y;

            if (mesh->HasTextureCoords(0))
            {
                aiVector3D uv = mesh->mTextureCoords[0][i];
                vertex.uv[0] = uv.x;
                vertex.uv[1] = uv.y;
            }
            else
            {
                vertex.uv[0] = 0;
                vertex.uv[1] = 0;
            }

            hro_mesh.vertices.push_back(vertex);
        }

        // Indices
        int mesh_index_count = 0;
        for (uint32_t i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];

            for (uint32_t j = 0; j < face.mNumIndices; j++)
            {
                hro_mesh.indices.push_back(face.mIndices[j]);
                mesh_index_count++;
            }
        }

        // Bone mapping
        if(mesh->HasBones())
        {
            // Resize bones array
            joints.resize(joints.size() + mesh->mNumBones);

            // Resize vertex to bone mapping
            vertex_to_joints.resize(vertex_to_joints.size() + mesh->mNumVertices);

            // Offset by node index
            for(uint32_t bone_id = 0; bone_id < mesh->mNumBones; bone_id++)
            {
                hro::VertexBoneData vertex_bone_data = {};
                aiBone* ai_bone = mesh->mBones[bone_id];

                // Push vertex to bone mapping
                for(uint32_t i = 0; i < ai_bone->mNumWeights; i++)
                {
                    uint32_t vertex_index = ai_bone->mWeights[i].mVertexId;
                    float bone_weight = ai_bone->mWeights[i].mWeight;

                    vertex_to_joints[vertex_index].AddWeight(bone_id, bone_weight);
                }

                // Push joint final transform
                aiMatrix4x4 inverse_bind_pose_transform = ai_bone->mOffsetMatrix;
                inverse_bind_pose_transform = inverse_bind_pose_transform.Transpose();

                hro::Joint joint = {};
                memcpy(joint.inverse_bind_pose_transform, &inverse_bind_pose_transform, sizeof(inverse_bind_pose_transform));
                memcpy(joint.name_buffer, &ai_bone->mName.data, std::min((uint32_t)sizeof(joint.name_buffer), ai_bone->mName.length));

                assert(joints.size() >= bone_id && "Bone id outside buffer bounds");
                joints[bone_id] = joint;

                // Node name to bone
                node_name_to_bones[std::string(ai_bone->mNode->mName.data)] = bone_id;
            }
        }

        hro::MeshInfo mesh_info = {};
        mesh_info.vertex_format = hro::VertexFormat::F32_PNCV;
        mesh_info.vertex_buffer_size = mesh->mNumVertices * sizeof(hro::Vertex_F32_PNCV);

        mesh_info.index_format = hro::IndexFormat::UINT32;
        mesh_info.index_buffer_size = mesh_index_count * sizeof(uint32_t);

        mesh_info.bone_count = mesh->mNumBones;

        mesh_info.name = mesh->mName.data;
        mesh_info.material_id = mesh->mMaterialIndex;
        mesh_info.original_file_path = "assets/meshes/" + std::string(mesh_info.name) + ".ymesh";

        // Copy global model matrix
        aiMatrix4x4 global_mesh_matrix = global_inverse_transform * global_node_model_matrix;
        global_mesh_matrix = global_mesh_matrix.Transpose();
        float* data = (float*)(&global_mesh_matrix);
        memcpy(mesh_info.model_matrix, data, sizeof(global_mesh_matrix));
      
        mesh_infos.push_back(mesh_info);
        meshes.push_back(hro_mesh);
    }

    void ProcessMaterials(const aiScene* scene)
    {
        for (uint32_t i = 0; i < scene->mNumMaterials; i++)
        {
            hro::Material material = {};

            const aiMaterial* ai_material = scene->mMaterials[i];
            auto path= std::filesystem::path(ai_material->GetName().data);
            path.replace_extension();
            std::string name = path.string();

            // Check if material name already taken
            bool processed = false;
            for (hro::Material& m : materials)
            {
                if (m.name.data() == name)
                {
                    processed = true;
                    break;
                }
            }

            if (processed)
            {
                std::string suff = "_" + std::to_string(i);
                name = name + suff;
            }

            // Colors
            aiColor3D ambient_color(1.0);
            if (ai_material->Get(AI_MATKEY_COLOR_AMBIENT, ambient_color) == AI_SUCCESS)
            {
                material.ambient_color[0] = ambient_color.r;
                material.ambient_color[1] = ambient_color.g;
                material.ambient_color[2] = ambient_color.b;
            }

            aiColor3D diffuse_color(1.0);
            if (ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color) == AI_SUCCESS)
            {
                material.diffuse_color[0] = diffuse_color.r;
                material.diffuse_color[1] = diffuse_color.g;
                material.diffuse_color[2] = diffuse_color.b;
            }

            aiColor3D specular_color(1.0);
            if (ai_material->Get(AI_MATKEY_COLOR_SPECULAR, specular_color) == AI_SUCCESS)
            {
                material.specular_color[0] = specular_color.r;
                material.specular_color[1] = specular_color.g;
                material.specular_color[2] = specular_color.b;
            }

            // Textures
            material.diffuse_texture_path = convert_material_texture(ai_material, aiTextureType_DIFFUSE, scene);
            material.specular_texture_path = convert_material_texture(ai_material, aiTextureType_SPECULAR, scene);

            material.name = name;
            materials.push_back(material);
        }
    }
private:
    // Returns converted texture path
    std::string convert_material_texture(const aiMaterial* material, aiTextureType type, const aiScene* scene);
private:
    // Global vertex to bone mapping buffer
	std::vector<hro::VertexBoneData> vertex_to_joints;

    // Global bones buffer
	std::vector<hro::Joint> joints;
private:
    std::unordered_map<std::string, uint32_t> node_name_to_bones;
    aiMatrix4x4 global_inverse_transform;
};