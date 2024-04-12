#pragma once

#include "Core/Memory.h"
#include "Resource/Resource.h"

namespace yoyo
{
    enum class TextureSamplerType
    {
        Nearest = 0,
        Linear,
    };

    enum class TextureAddressMode
    {
        Repeat = 0,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
        MirrorClampToEdge,
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
        AddressMode = 1 << 2,
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

        const TextureSamplerType GetSamplerType() const { return m_sampler_type; }
        void SetSamplerType(TextureSamplerType type);

        const TextureAddressMode GetAddressMode() const { return m_sampler_address_mode; }
        void SetAddressMode(TextureAddressMode mode);

        virtual void UploadTextureData(bool free_host_memory = false) = 0;

        static Ref<Texture> Create(const std::string& name = "");
        static Ref<Texture> LoadFromAsset(const char* asset_path);

        float width;
        float height;

        const TextureDirtyFlags& DirtyFlags() { return m_dirty; }

        TextureFormat format;
        std::vector<char> raw_data;
        friend class ResourceManager;
    protected:
        TextureSamplerType m_sampler_type;
        TextureAddressMode m_sampler_address_mode;

        bool m_live;
        TextureDirtyFlags m_dirty;
    };
}

template<>
struct std::hash<yoyo::Texture>
{
    std::size_t operator()(const yoyo::Texture& texture) const noexcept
    {
        // Compute individual hash values for first, second and third
        // http://stackoverflow.com/a/1646913/126995

        std::size_t res = texture.width * texture.height;
        res = res * 31 + hash<int>()((int)texture.format);
        res = res * 31 + hash<int>()((int)texture.GetSamplerType());
        res = res * 31 + hash<int>()((int)texture.GetAddressMode());
        res = res * 31 + hash<string>()(texture.name);

        return res;
    }
};