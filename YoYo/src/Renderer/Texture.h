#pragma once

#include "Core/Memory.h"

#include "Resource/Resource.h"

namespace yoyo
{
    enum class TextureSamplerType
    {
        Linear,
        Nearest,
    };

    enum class TextureFormat 
    {
        Unknown, 
        R8,
        RGB8,
        RGBA8
    };

    enum class TextureDirtyFlags : uint16_t
    {
        Clean = 0,
        Unuploaded = 1 << 0,
        SamplerType = 1 << 1,
    };

    inline TextureDirtyFlags operator~ (TextureDirtyFlags a) { return (TextureDirtyFlags)~(int)a; }
    inline TextureDirtyFlags operator| (TextureDirtyFlags a, TextureDirtyFlags b) { return (TextureDirtyFlags)((int)a | (int)b); }
    inline TextureDirtyFlags operator& (TextureDirtyFlags a, TextureDirtyFlags b) { return (TextureDirtyFlags)((int)a & (int)b); }
    inline TextureDirtyFlags operator^ (TextureDirtyFlags a, TextureDirtyFlags b) { return (TextureDirtyFlags)((int)a ^ (int)b); }
    inline TextureDirtyFlags& operator|= (TextureDirtyFlags& a, TextureDirtyFlags b) { return (TextureDirtyFlags&)((int&)a |= (int)b); }
    inline TextureDirtyFlags& operator&= (TextureDirtyFlags& a, TextureDirtyFlags b) { return (TextureDirtyFlags&)((int&)a &= (int)b); }
    inline TextureDirtyFlags& operator^= (TextureDirtyFlags& a, TextureDirtyFlags b) { return (TextureDirtyFlags&)((int&)a ^= (int)b); }

    // The base texture class 
    class YAPI Texture : public Resource
    {
    public:
        RESOURCE_TYPE(Texture)

        Texture() = default;
        ~Texture() = default;

        const TextureSamplerType GetSamplerType() const {return m_sampler_type;}
        void SetSamplerType(TextureSamplerType type);

        virtual void UploadTextureData(bool free_host_memory = false) = 0;

        static Ref<Texture> Create(const std::string& name = "");
        static Ref<Texture> LoadFromAsset(const char* asset_path);

        float width;
        float height;

        const TextureDirtyFlags& DirtyFlags() {return m_dirty;}

        TextureFormat format;
        std::vector<char> raw_data;
    protected:
        friend class ResourceManager;
        TextureSamplerType m_sampler_type;

        bool m_live;
        TextureDirtyFlags m_dirty;
    };
}