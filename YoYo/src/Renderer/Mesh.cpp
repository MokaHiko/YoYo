#include "Mesh.h"
#include "Resource/ResourceManager.h"

#include "Core/Log.h"


namespace yoyo
{
    template<>
    YAPI Ref<Mesh> ResourceManager::Load<Mesh>(const std::string& path)
    {
		const std::string name = FileNameFromFullPath(path);

        auto& mesh_cache = Cache<Mesh>();
		auto mesh_it = std::find_if(mesh_cache.begin(), mesh_cache.end(), [&](const auto& it){
			return it.second->name == name;
		});
		
		if(mesh_it != mesh_cache.end())
		{
			return mesh_it->second;
		}

        // TODO: Load from asset file
        YTRACE("[Cache Miss][Mesh]: %s", name.c_str());
     
        return nullptr;
    }

    template<>
    YAPI void ResourceManager::Free<Mesh>(Ref<Mesh> resource)
    {
        // TODO: Free resource
    }

    void Mesh::RecalculateNormals()
    {
        m_dirty |= MeshDirtyFlags::VertexDataChange;
    }
}