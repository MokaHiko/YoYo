#include "Shader.h"

#include "Core/Log.h"
#include "Resource/ResourceManager.h"

namespace yoyo
{
    template<>
    Ref<Shader> ResourceManager::Load<Shader>(const std::string& path)
    {
        ResourceId id = FileNameFromFullPath(path);

        auto shader_it = m_shader_cache.find(id);
        if (shader_it != m_shader_cache.end())
        {
            return shader_it->second;
        }

        YTRACE("Cache Miss!");
        // TODO: Load from asset file
        // auto& shader = Shader::LoadFromAsset(path.c_str());
        // if(!shader)
        // {
        // 	return nullptr;
        // }

        // shader->dirty = ShaderDirtyFlags::Unuploaded;
        // m_shader_cache[shader->m_id] = shader;
        // return shader;

        return nullptr;
    }

    template<>
    void ResourceManager::Free<Shader>(Ref<Shader> resource)
    {
        // TODO: Free resource
    }

    bool ResourceManager::OnShaderCreated(Ref<Shader> shader)
    {
        if (shader->ID().empty())
        {
            // TODO: Generate string uuid
        }

        // Check if shader already cached
        auto shader_it = m_shader_cache.find(shader->ID());
        if (shader_it != m_shader_cache.end())
        {
            return true;
        }

        // TODO: Check if there is space in cache

        // Cache
        m_shader_cache[shader->ID()] = shader;
        return true;
    }

    Shader::Shader()
    {
    }

    Shader::~Shader()
    {
    }
}