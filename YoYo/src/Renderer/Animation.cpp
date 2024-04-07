#include "Animation.h"

#include <Hurno.h>

#include "Core/Assert.h"
#include "Math/MatrixTransform.h"
#include "Math/Quaternion.h"

#include "Renderer/SkinnedMesh.h"
#include "Events/Event.h"

#include "Resource/ResourceManager.h"
#include "Resource/ResourceEvent.h"

namespace yoyo
{
	const Vec3 GetInterpolatedPosition(const Ref<Animation>& animation, int index, float time_in_ticks)
	{
		if (animation->position_channels.size() == 1)
		{
			Vec3 position = {position.x = animation->position_channels[0].data[0],
							position.y = animation->position_channels[0].data[1],
							position.z = animation->position_channels[0].data[2] };

			return position;
		}
		
		for (int i = 1; i < animation->position_channels.size(); i++)
		{
			if(animation->position_channels[i].time > time_in_ticks && animation->position_channels[i].bone_index == index)
			{
				auto& pos_channel_1 = animation->position_channels[i - 1];
				float t1 = pos_channel_1.time;

				auto& pos_channel_2 = animation->position_channels[i];
				float t2 = pos_channel_2.time;

				Vec3 pos1 = {};
				pos1.x = pos_channel_1.data[0];
				pos1.y = pos_channel_1.data[1];
				pos1.z = pos_channel_1.data[2];

				Vec3 pos2 = {};
				pos2.x = pos_channel_2.data[0];
				pos2.y = pos_channel_2.data[1];
				pos2.z = pos_channel_2.data[2];

				float dt = t2 - t1;
				float factor = (time_in_ticks - t1) / dt;

				return Lerp(pos1, pos2, factor);
			}
		}

		return {0.0f, 0.0f, 0.0f};
	}

	const Quat GetInterpolatedRotation(const Ref<Animation>& animation, int index, float time_in_ticks)
	{
		if (animation->rotation_channels.size() == 1)
		{
			Quat rotation = {};
			memcpy(&rotation, animation->rotation_channels[0].data, animation->rotation_channels[0].SizeOfData());
			return rotation;
		}

		for (int i = 1; i < animation->rotation_channels.size(); i++)
		{
			if(animation->rotation_channels[i].time > time_in_ticks && animation->rotation_channels[i].bone_index == index)
			{
				const auto& rot_channel_1 = animation->rotation_channels[i - 1];
				float t1 = rot_channel_1.time;

				const auto& rot_channel_2 = animation->rotation_channels[i];
				float t2 = rot_channel_2.time;

				Quat rot1 = {};
				rot1.x = rot_channel_1.data[0];
				rot1.y = rot_channel_1.data[1];
				rot1.z = rot_channel_1.data[2];
				rot1.w = rot_channel_1.data[3];

				Quat rot2 = {};
				rot2.x = rot_channel_2.data[0];
				rot2.y = rot_channel_2.data[1];
				rot2.z = rot_channel_2.data[2];
				rot2.w = rot_channel_2.data[3];

				float dt = t2 - t1;
				float factor = (time_in_ticks - t1) / dt;

				return Slerp(rot1, rot2, factor);
			}
		}

		return {0.0f, 0.0f, 0.0f, 1.0f};
	}

	const Vec3 GetInterpolatedScale(const Ref<Animation>& animation, int index, float time_in_ticks)
	{
		if (animation->scale_channels.size() == 1)
		{
			Vec3 scale = {};
			memcpy(&scale, animation->scale_channels[0].data, animation->scale_channels[0].SizeOfData());

			return scale;
		}

		for (int i = 1; i < animation->scale_channels.size(); i++)
		{
			if(animation->scale_channels[i].time > time_in_ticks && animation->scale_channels[i].bone_index == index)
			{
				const auto& scale_channel_1 = animation->scale_channels[i - 1];
				float t1 = scale_channel_1.time;

				const auto& scale_channel_2 = animation->scale_channels[i];
				float t2 = scale_channel_2.time;

				Vec3 scale1 = {};
				scale1.x = scale_channel_1.data[0];
				scale1.y = scale_channel_1.data[1];
				scale1.z = scale_channel_1.data[2];

				Vec3 scale2 = {};
				scale2.x = scale_channel_2.data[0];
				scale2.y = scale_channel_2.data[1];
				scale2.z = scale_channel_2.data[2];

				float dt = t2 - t1;
				float factor = (time_in_ticks - t1) / dt;
				
				return Lerp(scale1, scale2, factor);
			}
		}

		return {1.0f, 1.0f, 1.0f};
	}

	Ref<Animation> Animation::Create(const std::string& name)
	{
		Ref<Animation> animation = CreateRef<Animation>();
		animation->name = name;

		EventManager::Instance().Dispatch(CreateRef<AnimationCreatedEvent>(animation));
		return animation;
	}

	template<>
	YAPI Ref<Animation> ResourceManager::Load<Animation>(const std::string& path)
	{
		const std::string name = FileNameFromFullPath(path);

		auto& animation_cache = Cache<Animation>();
		auto animation_it = std::find_if(animation_cache.begin(), animation_cache.end(), [&](const auto& it) {
			return it.second->name == name;
			});

		if (animation_it != animation_cache.end())
		{
			return animation_it->second;
		}

		YWARN("[Cache Miss][Animation]: %s", name.c_str());
		Ref<Animation> animation = Animation::LoadFromAsset(path.c_str(), name);

		if (!animation)
		{
			return nullptr;
		}

		animation_cache[animation->Id()] = animation;

		return animation;
	}

	Ref<Animation> Animation::LoadFromAsset(const char* asset_path, const std::string& name)
	{
		hro::Animation hro_animation = {};
		if (hro_animation.Load(asset_path))
		{
			hro_animation.Unpack(nullptr, nullptr);

			// Copy animation data
			const std::string asset_name = name.empty() ? hro_animation.name : name;
			Ref<Animation> animation = Animation::Create(asset_name);
			animation->ticks = hro_animation.ticks;
			animation->ticks_per_second = hro_animation.ticks_per_second;

			animation->position_channels.resize(hro_animation.position_channels.size());
			memcpy(animation->position_channels.data(), hro_animation.position_channels.data(), hro_animation.position_channels.size() * sizeof(hro_animation.position_channels[0]));

			animation->rotation_channels.resize(hro_animation.rotation_channels.size());
			memcpy(animation->rotation_channels.data(), hro_animation.rotation_channels.data(), hro_animation.rotation_channels.size() * sizeof(hro_animation.rotation_channels[0]));

			animation->scale_channels.resize(hro_animation.scale_channels.size());
			memcpy(animation->scale_channels.data(), hro_animation.scale_channels.data(), hro_animation.scale_channels.size() * sizeof(hro_animation.scale_channels[0]));

			return animation;
		}

		return nullptr;
	}

	void Animator::Play(uint32_t animation_index)
	{
		if (animation_index >= animations.size())
		{
			YERROR("Uknown animation index!");
			return;
		}

		m_current_animation = animation_index;
	}

	void Animator::Update(float dt)
	{
		if (!skinned_mesh)
		{
			YWARN("Animation cannot play without skinned mesh attached!");
			return;
		}

		if (!skinned_mesh->skeletal_hierarchy)
		{
			YWARN("Animation cannot play without skeletal hierarchy!");
			return;
		}

		if (animations.empty())
		{
			return;
		}

		YASSERT(m_current_animation < animations.size(), "Uknown animation index!");

		Ref<Animation> animation = animations[m_current_animation];
		if(!animation)
		{
			YWARN("Animation selected is null reference!");
			return;
		}

		float time_in_ticks = m_time_elapsed * animation->ticks_per_second;

		Mat4x4 identity = {};
		UpdateHierarchyRecursive(skinned_mesh->skeletal_hierarchy->GetRoot(), identity, time_in_ticks);

		m_time_elapsed += dt;
		if (m_time_elapsed > animation->Duration())
		{
			m_time_elapsed = 0.0f;

			// Check if go next
			// m_current_animation = (m_current_animation + 1) % animations.size();
		}
	}

	void Animator::Stop()
	{
	}

	void Animator::UpdateHierarchyRecursive(const SkeletalNode* node, const Mat4x4& parent_transform, float time_in_ticks)
	{
		Mat4x4 node_transform = node->transform;

		const Ref<Animation>& animation = animations[m_current_animation];

		// check if node is animated (i.e has joint index)
		if (node->joint_index >= 0)
		{
			Vec3 position = GetInterpolatedPosition(animation, node->joint_index, time_in_ticks);
			Quat rotation = GetInterpolatedRotation(animation, node->joint_index, time_in_ticks);
			Vec3 scale = GetInterpolatedScale(animation, node->joint_index, time_in_ticks);

			node_transform = yoyo::TranslationMat4x4(position) * yoyo::TransposeMat4x4(yoyo::QuatToMat4x4(rotation)) * yoyo::ScaleMat4x4(scale);
		}

		Mat4x4 global_transform = parent_transform * node_transform;
		if (node->joint_index >= 0)
		{
			const SkinnedMeshJoint& joint = skinned_mesh->joints[node->joint_index];
			skinned_mesh->bones[node->joint_index] = global_transform * joint.inverse_bind_pose_transform;
		}

		for (const SkeletalNode* child : node->children)
		{
			UpdateHierarchyRecursive(child, global_transform, time_in_ticks);
		}
	}
}