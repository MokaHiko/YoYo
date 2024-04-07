#pragma once

#include "Defines.h"
#include "Core/Memory.h"

#include "Math/Math.h"
#include "Math/Quaternion.h"
#include "Resource/Resource.h"

namespace yoyo
{
    template<typename T>
    struct YAPI Channel
    {
        uint32_t bone_index = 0;
        float time = 0;
        T data = {};

        const uint64_t SizeOfData() const {return sizeof(T);}
    };

    class YAPI Animation : public Resource
    {
    public:
        RESOURCE_TYPE(Animation)

        static Ref<Animation> Create(const std::string& name = "");
        static Ref<Animation> LoadFromAsset(const char* asset_path, const std::string& name = "");

        // Returns the duration of the animation in seconds
        const float Duration() const { return ticks / ticks_per_second; }

        std::vector<Channel<float[3]>> position_channels = {};
        std::vector<Channel<float[4]>> rotation_channels = {};
        std::vector<Channel<float[3]>> scale_channels = {};

        float ticks;
        float ticks_per_second;
    };

    class SkinnedMesh;
    class SkeletalNode;
    class YAPI Animator
    {
    public:
        std::vector<Ref<Animation>> animations;
        Ref<SkinnedMesh> skinned_mesh;

        void Play(uint32_t animation_index);
        void Update(float dt);
        void Stop();

        void SetTime(float time) {m_time_elapsed = time;}
        const float GetTime() const {return m_time_elapsed;}

        const uint32_t GetCurrentAnimationIndex() const {return m_current_animation;}
    private:
        void UpdateHierarchyRecursive(const SkeletalNode* node, const Mat4x4& parent_transform, float time_in_ticks);

        // The index of the current animation
        uint32_t m_current_animation = 0;

        // The time elapsed in seconds
        float m_time_elapsed = 0;
    };
}