#include "Asset.h"

#include <fstream>
#include <assert.h>

namespace hro
{
	void Asset::Pack(const AssetInfo* info, void* raw_data, size_t raw_data_size)
	{
		assert(info != nullptr, && "Info passed cannot be nullptr!");
		if(IsPacked())
		{
			printf("Asset already packed!\n");
			return;
		}

		PackImpl(info, raw_data, raw_data_size);
		m_packed = true;
	}

	void Asset::Pack(const AssetInfo* info)
	{
		assert(info != nullptr, && "Info passed cannot be nullptr!");
		if(IsPacked())
		{
			printf("Asset already packed!\n");
			return;
		}

		PackImpl(info);
		m_packed = true;
	}

	void Asset::Pack()
	{
		if(IsPacked())
		{
			printf("Asset already packed!\n");
			return;
		}

		PackImpl();
		m_packed = true;
	}

	void Asset::Unpack(const AssetInfo* info, void* dst_buffer)
	{
		if(!IsPacked())
		{
			printf("Asset already unpacked!\n");
			return;
		}

		UnpackImpl(info, dst_buffer);
		m_packed = false;
	}

	bool Asset::Load(const char* path)
	{
		std::ifstream file;
		file.open(path, std::ios::binary);

		if (!file.is_open())
		{
			return false;
		}

		file.seekg(0);

		file.read(type, ASSET_TYPE_SIZE);
		file.read((char*)&version, sizeof(uint32_t));

		uint32_t meta_data_size = 0;
		file.read((char*)&meta_data_size, sizeof(uint32_t));

		uint32_t packed_data_size = 0;
		file.read((char*)&packed_data_size, sizeof(uint32_t));

		json_meta_data.resize(static_cast<size_t>(meta_data_size));
		file.read((char*)json_meta_data.data(), meta_data_size);

		// Read in packed data
		packed_data.resize(static_cast<size_t>(packed_data_size));
		file.read(packed_data.data(), packed_data_size);
		m_packed = true;

		file.close();
		
		return m_loaded = true;
	}

	bool Asset::Save(const char* path)
	{
		if(!IsPacked())
		{
			printf("Asset cannot be saved without upacked!\n");
			return false;
		}

		std::ofstream file;
		file.open(path, std::ios::binary | std::ios::out);

		if (!file.is_open())
		{
			return false;
		}

		file.write(type, ASSET_TYPE_SIZE);
		file.write((const char*)&version, sizeof(uint32_t));

		// Meta data size
		uint32_t meta_data_size = json_meta_data.size();
		file.write((const char*)&meta_data_size, sizeof(uint32_t));

		// Packed data size
		uint32_t packed_data_size = packed_data.size();
		file.write((const char*)&packed_data_size, sizeof(uint32_t));

		file.write(json_meta_data.data(), meta_data_size);
		file.write(packed_data.data(), packed_data_size);

		file.close();

		return true;
	}

	CompressionMode ParseCompressionMode(const char* f) 
	{
        if (strcmp(f, "LZ4") == 0)
            return CompressionMode::LZ4;
        else
            return CompressionMode::None;
	}
	const std::string FileNameFromFullPath(const std::string& full_path)
    {
        auto first = full_path.find_last_of("/\\") + 1;
        auto last = full_path.find_last_of(".");

        auto it = last - first;
        return full_path.substr(first, last - first);
    }
} 
