#include "Mesh.h"
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

        // TODO: Load from asset file
        YWARN("[Cache Miss][StaticMesh]: %s", name.c_str());
     
        return nullptr;
    }

    template<>
    YAPI void ResourceManager::Free<StaticMesh>(Ref<StaticMesh> resource)
    {
        // TODO: Free resource
    } 
}