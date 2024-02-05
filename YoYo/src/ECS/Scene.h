#pragma once

#include "Defines.h"
#include "Math/Math.h"

#include "Entity.h"
#include "EntityImpl.h"

namespace yoyo
{
    class YAPI Scene
    {
    public:
        Scene();
        ~Scene();

        template <typename T, typename... Args>
        T& AddComponent(entt::entity id, Args &&...args)
        {
            if (HasComponent<T>(id))
            {
                return GetComponent<T>(id);
            }

            return m_registry.emplace<T>(id, std::forward<Args>(args)...);
        }

        template <typename T, typename... Args>
        bool RemoveComponent(entt::entity id, Args &&...args)
        {
            if (HasComponent<T>(id))
            {
                return false;
            }

            m_registry.remove<T>(id);
            return true;
        }

        template <typename T>
        bool HasComponent(entt::entity id)
        {
            return m_registry.any_of<T>(id);
        }

        template <typename T>
        T& GetComponent(entt::entity id)
        {
            if (HasComponent<T>(id))
            {
                return m_registry.get<T>(id);
            }

            YERROR("Entity has no such component!");
            throw std::runtime_error("Entity has no such component!");
        }

        // Create and returns entity
        Entity Instantiate(const std::string& name = "", const Vec3& position = {}, bool* serialize = nullptr);

        entt::registry& Registry();
    private:
        entt::registry m_registry;
    };
}