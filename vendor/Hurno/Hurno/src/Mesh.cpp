#include "Mesh.h"

#include <nlohmann/json.hpp>
#include <lz4.h>

namespace hro
{
	const uint32_t MeshInfo::GetVertexCount() const
	{
		uint64_t vertex_size = 0.0f;

		switch (vertex_format)
		{
		case (VertexFormat::F32_PNCV):
			vertex_size = sizeof(Vertex_F32_PNCV);
			break;
		default:
			printf("Uknown vertex format!");
			break;
		}

		return static_cast<uint32_t>(vertex_buffer_size / vertex_size);
	}

	const uint32_t MeshInfo::GetIndexCount() const
	{
		uint64_t index_size = 0.0f;

		switch (index_format)
		{
		case (IndexFormat::UINT32):
			index_size = sizeof(uint32_t);
			break;
		default:
			printf("Uknown index format!");
			break;
		}

		return static_cast<uint32_t>(index_buffer_size / index_size);
	}

    void Mesh::ParseInfo(AssetInfo* out)
	{
	};

	void Mesh::PackImpl()
	{
		type[0] = 'M';
		type[1] = 'O';
		type[2] = 'D';
		type[3] = 'L';

		version = 1;
		
		uint64_t vertex_buffer_size = vertices.size() * sizeof(Vertex_F32_PNCV);
		uint64_t index_buffer_size = indices.size() * sizeof(uint32_t);
		uint64_t buffer_size = vertex_buffer_size + index_buffer_size;

		nlohmann::json meta_data = {};
		meta_data["vertex_buffer_size"] = vertex_buffer_size;
		meta_data["index_buffer_size"] = index_buffer_size;
		meta_data["original_file_path"] = "";

		meta_data["compression_mode"] = "LZ4";
		json_meta_data = meta_data.dump();

		void* data = malloc(buffer_size);
		memcpy(data, vertices.data(), vertex_buffer_size);
		memcpy((char*)data + vertex_buffer_size, indices.data(), index_buffer_size);

		int compressed_bound = LZ4_compressBound(buffer_size);
		packed_data.resize(compressed_bound);
		int compressed_size = LZ4_compress_default((const char*)data, packed_data.data(), buffer_size, compressed_bound);
		packed_data.resize(compressed_size);

		free(data);
	}

	void Mesh::UnpackImpl(const AssetInfo* in, void* dst_buffer) 
	{
		nlohmann::json meta_data = nlohmann::json::parse(json_meta_data);

		CompressionMode compression_mode = ParseCompressionMode(std::string(meta_data["compression_mode"]).c_str());

		uint64_t vertex_buffer_size = meta_data["vertex_buffer_size"];
		uint64_t index_buffer_size = meta_data["index_buffer_size"];
		uint64_t buffer_size = vertex_buffer_size + index_buffer_size;

		uint64_t vertex_count = vertex_buffer_size  / sizeof(Vertex_F32_PNCV);
		uint64_t index_count = index_buffer_size / sizeof(uint32_t);

		meta_data["compression_mode"] = "LZ4";

		void *data = malloc(buffer_size);
		if (compression_mode == CompressionMode::LZ4)
		{
			LZ4_decompress_safe(packed_data.data(), (char*)data, packed_data.size(), buffer_size);
		}
		else
		{
			// NO compression
			memcpy(data, packed_data.data(), packed_data.size());
		}

		vertices.resize(vertex_count);
		memcpy(vertices.data(), data, vertex_buffer_size);

		indices.resize(index_count);
		memcpy(indices.data(), (char*)data + vertex_buffer_size, index_buffer_size);

		free(data);
	}
}
