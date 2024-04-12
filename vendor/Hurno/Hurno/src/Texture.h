#pragma once

#include "Defines.h"

#include <string.h>
#include <cstdint>

#include "Asset.h"

namespace hro
{
    enum class TextureFormat : uint8_t
    {
        Unknown, 
        R8,
        RGB8,
        RGBA8
    };

	TextureFormat ParseTextureFormat(const char* f);

    struct TextureInfo : public AssetInfo
    {
        TextureFormat format; // Format of the texture
        uint64_t size; // Size of the texture in bytes
        uint32_t pixel_size[2]; // width, height 

        // Returns the size of raw data when unpacked
        virtual const uint64_t UnpackedSize() const override 
        {
            return size;
        }
    };

    class HAPI Texture : public Asset
    {
    public:
        Texture() = default;
        virtual ~Texture() = default;

        virtual void ParseInfo(AssetInfo* out) override;
    protected:
        virtual void PackImpl(const AssetInfo* in, void* raw_data, size_t raw_data_size) override;
        virtual void UnpackImpl(const AssetInfo* in, void* dst_buffer) override;
    };
}