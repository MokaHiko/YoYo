#include "Mesh.h"
#include "Resource/ResourceManager.h"

#include "Core/Log.h"


namespace yoyo
{
    template<>
    YAPI Ref<Mesh> ResourceManager::Load<Mesh>(const std::string& path)
    {
        ResourceId id = FileNameFromFullPath(path);

        auto mesh_it = m_mesh_cache.find(id);
        if (mesh_it != m_mesh_cache.end())
        {
            return mesh_it->second;
        }

        YTRACE("[Cache Miss]: Resource Type: Mesh!");
        // TODO: Load from asset file
        // auto& mesh = Mesh::LoadFromAsset(path.c_str());
        // if(!mesh)
        // {
        // 	return nullptr;
        // }

        // mesh->dirty = MeshDirtyFlags::Unuploaded;
        // m_mesh_cache[mesh->m_id] = mesh;
        // return mesh;

        return nullptr;
    }

    template<>
    YAPI void ResourceManager::Free<Mesh>(Ref<Mesh> resource)
    {
        // TODO: Free resource
    }

    bool ResourceManager::OnMeshCreated(Ref<Mesh> mesh)
    {
        // Check if mesh already cached
        if (mesh->ID().empty())
        {
            // TODO: Generate string uuid
        }
        else
        {
            auto mesh_it = m_mesh_cache.find(mesh->ID());
            if (mesh_it != m_mesh_cache.end())
            {
                return true;
            }
        }

        // TODO: Check if there is space in cache
        m_mesh_cache[mesh->ID()] = mesh;

        return false;
    }

    void Mesh::RecalculateNormals()
    {
        m_dirty |= MeshDirtyFlags::VertexDataChange;
    }
}