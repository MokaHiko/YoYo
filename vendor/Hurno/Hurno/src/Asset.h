#pragma once

#include "Defines.h"

#include <string>
#include <vector>

namespace hro
{
    enum class CompressionMode : uint8_t
    {
        None, 
        LZ4
    };

    CompressionMode ParseCompressionMode(const char *f);
    const std::string FileNameFromFullPath(const std::string& full_path);

    const int ASSET_TYPE_SIZE = 4;
    struct AssetInfo
    {
        CompressionMode compression_mode;
        uint64_t raw_data_size = 0;
        std::string original_file_path;

        virtual const std::string Name() const {return original_file_path;}

        // Returns the size of raw data when unpacked
        virtual const uint64_t UnpackedSize() const {return raw_data_size;}
    };

    class HAPI Asset
    {
    public:
        char type[ASSET_TYPE_SIZE];
        uint32_t version;
        std::string json_meta_data;
        std::vector<char> packed_data;

        Asset() = default;
        virtual ~Asset() = default;
    public:
        // Returns true if asset and asset info was loaded and parsed successfully.
        // Loaded assets are in a packed(compressed) state by default.
        bool Load(const char* path);

        // Returns true if asset was saved successfully 
        bool Save(const char* path);

        // Parses the model meta data into out 
        virtual void ParseInfo(AssetInfo* out) = 0;

        // Packs raw data into asset
        void Pack(const AssetInfo* info, void* raw_data, size_t raw_data_size);

        // Packs raw data into asset
        void Pack(const AssetInfo* info);

        // Convert all to no argument
        void Pack();

        // Unpacks asset raw data into dst buffer
        void Unpack(const AssetInfo* info, void* dst_buffer);

        // Returns true if asset has been loaded.
        const bool IsLoaded() const {return m_loaded;}

        // Returns whether or not an asset has been uncompressed and ready for use
        const bool IsPacked() const {return m_packed;}
    protected:
        // Convert all to no argument
        virtual void PackImpl() {};

        virtual void PackImpl(const AssetInfo* info) {};
        virtual void PackImpl(const AssetInfo* in, void* raw_data, size_t raw_data_size) {};
        virtual void UnpackImpl(const AssetInfo* in, void* dst_buffer) = 0;
    private:
        bool m_loaded = false;
        bool m_packed = false;
    };
}