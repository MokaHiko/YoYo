#include "Mesh.h"

#include <Hurno.h>

#include "Resource/ResourceManager.h"
#include "Core/Log.h"

namespace yoyo
{
    template<>
    YAPI Ref<StaticMesh> ResourceManager::Load<StaticMesh>(const std::string& path)
    {
		const std::string name = FileNameFromFullPath(path);

        auto& mesh_cache = Cache<StaticMesh>();
		auto mesh_it = std::find_if(mesh_cache.begin(), mesh_cache.end(), [&](const auto& it){
			return it.second->name == name;
		});
		
		if(mesh_it != mesh_cache.end())
		{
			return mesh_it->second;
		}

        YWARN("[Cache Miss][StaticMesh]: %s", name.c_str());

		Ref<StaticMesh> mesh = StaticMesh::LoadFromAsset(path.c_str());
		if (!mesh)
		{
			return nullptr;
		}

		mesh_cache[mesh->Id()] = mesh;
		return mesh;
    }

    Ref<StaticMesh> StaticMesh::LoadFromAsset(const char* asset_path, const std::string& name)
    {
        hro::Mesh hro_mesh;

        if(hro_mesh.Load(asset_path))
        {
            hro_mesh.Unpack(nullptr, nullptr);

			// Create and copy mesh
            const std::string static_mesh_name = name.empty() ? FileNameFromFullPath(asset_path) : name;
            Ref<StaticMesh> static_mesh = StaticMesh::Create(static_mesh_name);

            static_mesh->vertices.resize(hro_mesh.vertices.size());
            memcpy(static_mesh->vertices.data(), hro_mesh.vertices.data(), hro_mesh.vertices.size() * sizeof(Vertex));

            static_mesh->indices.resize(hro_mesh.indices.size());
            memcpy(static_mesh->indices.data(), hro_mesh.indices.data(), hro_mesh.indices.size() * sizeof(uint32_t));

            return static_mesh;
        }

        return nullptr;
    }

    template<>
    YAPI void ResourceManager::Free<StaticMesh>(Ref<StaticMesh> resource)
    {
        // TODO: Free resource
    } 

	Ref<StaticMesh> StaticMesh::CreateFromBuffers(const std::string& name, void* vertex_buffer, uint64_t vertex_buffer_size, void* index_buffer, uint64_t index_buffer_size)
    {
		Ref<StaticMesh> mesh = StaticMesh::Create(name);
		mesh->vertices.resize(vertex_buffer_size / sizeof(Vertex));
		memcpy(mesh->vertices.data(), (char*)vertex_buffer, vertex_buffer_size);

		mesh->indices.resize(index_buffer_size / sizeof(uint32_t));
		memcpy(mesh->indices.data(), (char*)index_buffer, index_buffer_size);

		return mesh;
    }
}