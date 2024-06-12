#include "Mesh.h"

#include <Hurno.h>

#include "Resource/ResourceManager.h"
#include "Core/Log.h"

#include "Resource/ResourceEvent.h"

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

    template<>
    YAPI void ResourceManager::Free<StaticMesh>(Ref<StaticMesh> resource)
    {
        // TODO: Free resource
    }

    Ref<StaticMesh> StaticMesh::Create(const std::string &name)
    {
        return std::static_pointer_cast<StaticMesh>(CreateImpl("StaticMesh", name));
    }

    Ref<StaticMesh> StaticMesh::LoadFromAsset(const char *asset_path, const std::string &name)
    {
        hro::Mesh hro_mesh;

        if(hro_mesh.Load(asset_path))
        {
            hro_mesh.Unpack(nullptr, nullptr);

			// Create and copy mesh
            const std::string static_mesh_name = name.empty() ? FileNameFromFullPath(asset_path) : name;
            auto static_mesh = StaticMesh::Create(static_mesh_name);

            static_mesh->GetVertices().resize(hro_mesh.vertices.size());
            memcpy(static_mesh->GetVertices().data(), hro_mesh.vertices.data(), hro_mesh.vertices.size() * sizeof(Vertex));

            static_mesh->GetIndices().resize(hro_mesh.indices.size());
            memcpy(static_mesh->GetIndices().data(), hro_mesh.indices.data(), hro_mesh.indices.size() * sizeof(uint32_t));

            return std::static_pointer_cast<StaticMesh>(static_mesh);
        }

        return nullptr;
    }

}