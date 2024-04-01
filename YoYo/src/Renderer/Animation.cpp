#include "Animation.h"

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
		
		for (int i = 1; animation->position_channels.size(); i++)
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

		YASSERT(0);
	}

	const Quat GetInterpolatedRotation(const Ref<Animation>& animation, int index, float time_in_ticks)
	{
		if (animation->rotation_channels.size() == 1)
		{
			Quat rotation = {};
			memcpy(&rotation, animation->rotation_channels[0].data, animation->rotation_channels[0].SizeOfData());
			return rotation;
		}

		for (int i = 1; animation->rotation_channels.size(); i++)
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

		YASSERT(0);
	}

	const Vec3 GetInterpolatedScale(const Ref<Animation>& animation, int index, float time_in_ticks)
	{
		if (animation->scale_channels.size() == 1)
		{
			Vec3 scale = {};
			memcpy(&scale, animation->scale_channels[0].data, animation->scale_channels[0].SizeOfData());

			return scale;
		}

		for (int i = 1; animation->scale_channels.size(); i++)
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

		YASSERT(0);
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
		//const std::string name = FileNameFromFullPath(path);
		const std::string name = path;

		auto& animation_cache = Cache<Animation>();
		auto animation_it = std::find_if(animation_cache.begin(), animation_cache.end(), [&](const auto& it) {
			return it.second->name == name;
			});

		if (animation_it != animation_cache.end())
		{
			return animation_it->second;
		}

		// TODO: Load from asset file
		YWARN("[Cache Miss][Animation]: %s", name.c_str());

		return nullptr;
	}

	void Animator::Play(uint32_t animation_index)
	{
		YASSERT(animation_index < animations.size(), "Uknown animation index!");
		m_current_animation;
	}

	void Animator::Update(float dt)
	{
		if (!skinned_mesh)
		{
			YWARN("Animation playing without skinned mesh attached!");
			return;
		}

		if (animations.empty())
		{
			return;
		}

		YASSERT(m_current_animation < animations.size(), "Uknown animation index!");

		const Ref<Animation>& animation = animations[m_current_animation];
		float time_in_ticks = m_time_elapsed * animation->ticks_per_second;

		Mat4x4 identity = {};
		UpdateHierarchyRecursive(skinned_mesh->skeletal_hierarchy->GetRoot(), identity, time_in_ticks);

		m_time_elapsed += dt;
		if (m_time_elapsed > animation->Duration())
		{
			m_time_elapsed = 0.0f;
		}
	}

	void Animator::Stop()
	{
	}

	void Animator::UpdateHierarchyRecursive(const SkeletalNode* node, const Mat4x4& parent_transform, float time_in_ticks)
	{
		Mat4x4 node_transform = node->transform;

		static const Ref<Animation>& animation = animations[m_current_animation];

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