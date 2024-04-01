#pragma once

#include <Math/Math.h>

#include "Entity.h"
#include "EntityImpl.h"

class Scene
{
public:
    Scene();
    ~Scene();

    Entity Root();

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
        if (!HasComponent<T>(id))
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

    template <typename T>
    Entity FindEntityWithComponent()
    {
        for(auto entity : m_registry.view<T>())
        {
            return Entity(entity, this);
        }

        return {};
    }

    // Create and returns entity
    Entity Instantiate(const std::string& name = "", const yoyo::Vec3& position = {}, bool* serialize = nullptr);
    Entity Instantiate(const std::string& name = "", const yoyo::Mat4x4& transform_matrix = {}, bool* serialize = nullptr);

    // Queues an entity for destruction
    void QueueDestroy(Entity e);

    void FlushDestructionQueue();

    void Destroy(Entity e);

    entt::registry& Registry();
private:
    std::vector<Entity> m_destruction_queue;

    Entity m_root;
    entt::registry m_registry;
};