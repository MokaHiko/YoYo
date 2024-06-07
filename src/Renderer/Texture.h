#pragma once

#include "Core/Memory.h"
#include "Resource/Resource.h"

namespace yoyo
{
    enum class TextureType : uint8_t
    {
        Uknown = 0,
        Color = 1 << 0,
        Depth = 1 << 1,
        Array = 1 << 2,
        CubeMap = 1 << 3
    };

    inline TextureType operator~(TextureType a) { return (TextureType) ~(int)a; }
    inline TextureType operator|(TextureType a, TextureType b) { return (TextureType)((int)a | (int)b); }
    inline TextureType operator&(TextureType a, TextureType b) { return (TextureType)((int)a & (int)b); }
    inline TextureType operator^(TextureType a, TextureType b) { return (TextureType)((int)a ^ (int)b); }
    inline TextureType &operator|=(TextureType &a, TextureType b) { return (TextureType &)((int &)a |= (int)b); }
    inline TextureType &operator&=(TextureType &a, TextureType b) { return (TextureType &)((int &)a &= (int)b); }
    inline TextureType &operator^=(TextureType &a, TextureType b) { return (TextureType &)((int &)a ^= (int)b); }

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

    constexpr char *TextureFormatStrings[] =
        {
            "Unknown",
            "R8",
            "RGB8",
            "RGBA8"};

    enum class TextureDirtyFlags : uint16_t
    {
        Clean = 0,
        Unuploaded = 1 << 0,
        SamplerType = 1 << 1,
        AddressMode = 1 << 2,
        DataChange = 1 << 3, // Data in the texture has been modified
        TypeChange = 1 << 4 // The type of texture 
    };

    inline TextureDirtyFlags operator~(TextureDirtyFlags a) { return (TextureDirtyFlags) ~(int)a; }
    inline TextureDirtyFlags operator|(TextureDirtyFlags a, TextureDirtyFlags b) { return (TextureDirtyFlags)((int)a | (int)b); }
    inline TextureDirtyFlags operator&(TextureDirtyFlags a, TextureDirtyFlags b) { return (TextureDirtyFlags)((int)a & (int)b); }
    inline TextureDirtyFlags operator^(TextureDirtyFlags a, TextureDirtyFlags b) { return (TextureDirtyFlags)((int)a ^ (int)b); }
    inline TextureDirtyFlags &operator|=(TextureDirtyFlags &a, TextureDirtyFlags b) { return (TextureDirtyFlags &)((int &)a |= (int)b); }
    inline TextureDirtyFlags &operator&=(TextureDirtyFlags &a, TextureDirtyFlags b) { return (TextureDirtyFlags &)((int &)a &= (int)b); }
    inline TextureDirtyFlags &operator^=(TextureDirtyFlags &a, TextureDirtyFlags b) { return (TextureDirtyFlags &)((int &)a ^= (int)b); }

    // The base texture class
    class YAPI Texture : public Resource
    {
    public:
        RESOURCE_TYPE(Texture)

        Texture();
        ~Texture();

        const TextureType GetTextureType() const { return m_type; }
        void SetTextureType(TextureType type);

        const TextureSamplerType GetSamplerType() const { return m_sampler_type; }
        void SetSamplerType(TextureSamplerType type);

        const TextureAddressMode GetAddressMode() const { return m_sampler_address_mode; }
        void SetAddressMode(TextureAddressMode mode);

        virtual void UploadTextureData(bool free_host_memory = false) = 0;

        // Creates a texture of type
        static Ref<Texture> Create(const std::string &name = "", TextureType type = TextureType::Color);
        static Ref<Texture> LoadFromAsset(const char *asset_path);

        int width;
        int height;

        // Used for array textures if greater than 1
        int layers;

        const TextureDirtyFlags &DirtyFlags() { return m_dirty; }

        TextureFormat format;
        std::vector<char> raw_data;
        friend class ResourceManager;
    protected:
        TextureType m_type;
        TextureSamplerType m_sampler_type;
        TextureAddressMode m_sampler_address_mode;

        bool m_live;
        TextureDirtyFlags m_dirty;
    };
}

template <>
struct std::hash<yoyo::Texture>
{
    std::size_t operator()(const yoyo::Texture &texture) const noexcept
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