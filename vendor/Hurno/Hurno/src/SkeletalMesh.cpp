#include "SkeletalMesh.h"

#include <nlohmann/json.hpp>
#include <lz4.h>

namespace hro
{
	void SkeletalMesh::ParseInfo(AssetInfo* out)
	{
		assert(out != nullptr && "Asset info pointer is null!");
		assert(json_meta_data.c_str() != nullptr && "Meta data string is null!");

		nlohmann::json texture_metadata = nlohmann::json::parse(json_meta_data);

		out->original_file_path = texture_metadata["original_file_path"];
		out->raw_data_size = texture_metadata["raw_data_size"];

		std::string compression_string = texture_metadata["compression_mode"];
		out->compression_mode = ParseCompressionMode(compression_string.c_str());
	}

	Node* SkeletalMesh::CreateNode()
	{
		assert(m_data != nullptr && "Data not allocated!");

		// Use memory newly allocate memory as node
		Node* node = (Node*)((char*)m_data + m_insert_index);
		assert(node != nullptr && "Failed to allocated node!");

		// Clear
		memset(node, 0, sizeof(Node));
		node->m_ptr = m_insert_index;

		// Update insert index
		m_insert_index += sizeof(Node);

		return node;
	}

	void SkeletalMesh::PackImpl(const AssetInfo* info)
	{
		type[0] = 'S';
		type[1] = 'K';
		type[2] = 'E';
		type[3] = 'L';

		version = 1;

		nlohmann::json meta_data = {};
		meta_data["raw_data_size"] = m_insert_index;
		meta_data["original_file_path"] = info->original_file_path;
		meta_data["compression_mode"] = "LZ4";
		json_meta_data = meta_data.dump();

		int compressed_bound = LZ4_compressBound(m_insert_index);
		packed_data.resize(compressed_bound);
		int compressed_size = LZ4_compress_default((const char*)m_data, packed_data.data(), m_insert_index, compressed_bound);
		packed_data.resize(compressed_size);

		if (m_data)
		{
			free(m_data);
		}
	}

	void SkeletalMesh::UnpackImpl(const AssetInfo* info, void* dst_buffer)
	{
		m_data = malloc(info->raw_data_size);
		if (info->compression_mode == CompressionMode::LZ4)
		{
			LZ4_decompress_safe(packed_data.data(), (char*)m_data, packed_data.size(), info->raw_data_size);
		}
		else
		{
			// NO compression
			memcpy(dst_buffer, packed_data.data(), packed_data.size());
		}
	}
}
