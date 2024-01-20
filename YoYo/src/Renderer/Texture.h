#pragma once

#include "Core/Memory.h"

namespace yoyo
{
    enum class TextureFormat : uint8_t
    {
        Unknown, 
        R8,
        RGB8,
        RGBA8
    };

    // The base texture class 
    class YAPI Texture
    {
    public:
        Texture() = default;
        ~Texture() {};

        static Ref<Texture> Create();
        static Ref<Texture> LoadFromAsset(const char* asset_path);

        float width;
        float height;

        TextureFormat format;
        std::vector<char> raw_data;
    protected:
        bool m_live;
    };
}