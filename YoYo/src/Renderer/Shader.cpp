#include "Shader.h"

#include "Core/Log.h"
#include "Resource/ResourceManager.h"

namespace yoyo
{
    template<>
    YAPI Ref<Shader> ResourceManager::Load<Shader>(const std::string& path)
    {
		const std::string name = FileNameFromFullPath(path);

        auto& shader_cache = Cache<Shader>();
		auto shader_it = std::find_if(shader_cache.begin(), shader_cache.end(), [&](const auto& it) {
			return it.second->name == name;
		});

		if (shader_it != shader_cache.end())
		{
			return shader_it->second;
		}

        YTRACE("[Cache Miss]: Resource Type: Shader!");
        return nullptr;
    }
}