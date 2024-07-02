#pragma once

#include "Defines.h"
#include "Core/Memory.h"
#include "Core/Layer.h"
#include "Core/Log.h"

#include "Resource.h"

namespace yoyo {
    // The application's main resource manager
    class YAPI ResourceManager
    {
    public:
        static ResourceManager& Instance();

        void Init();
        void Shutdown();

        template<typename T>
        Ref<T> Load(const std::string& path);

        template<typename T>
        void Free(Ref<T> resource);

        void Update();

        template<typename T>
        bool OnResourceCreated(Ref<T> resource)
        {
            // Check if resource cached
            auto& m_resource_cache = Cache<T>();
            if (resource->Id() != INVALID_RESOURCE_ID)
            {
                auto resource_it = std::find_if(Cache<T>().begin(), Cache<T>().end(), [&](const auto& it) {return resource == it.second; });
                if (resource_it != m_resource_cache.end())
                {
                    return true;
                }
            }
            else
            {
                resource->m_id = Platform::GenerateUUIDV4();
            }

            Cache<T>()[resource->Id()] = resource;
            return false;
        };

        template<typename T>
        std::unordered_map<ResourceId, Ref<T>>& Cache()
        {
            using ResourceCache = std::unordered_map<ResourceId, Ref<T>>;
            static ResourceCache* cache = nullptr;

            if(!cache)
            {
                cache = new ResourceCache;

                // TODO: Remove
                //YINFO("Resource Cache %s[%lu] Generated", T::s_resource_type_name.c_str(), T::s_resource_type);

                YINFO("Resource Cache %s[%lu] Generated", T::s_resource_type_name, T::s_resource_type);
            }

            return *cache;
        }

    private:
        ResourceManager();
        ~ResourceManager();
    };

    class Event;

    // Manages the runtime resources of the application (meshes, textures, audio, etc...) via a static/singleton ResourceManager 
    class YAPI RuntimeResourceLayer : public Layer
    {
    public:
        RuntimeResourceLayer() = default;
        virtual ~RuntimeResourceLayer();

        virtual void OnAttach() override;
        virtual void OnDetatch() override;

        virtual void OnUpdate(float dt) override;

        virtual void OnEnable() override;
        virtual void OnDisable() override;

        virtual void OnImGuiRender() override;

        LayerType(RuntimeResourceLayer)
    };
}