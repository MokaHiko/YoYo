#pragma once

#include <entt/entt.hpp>
#include <Core/Log.h>

class Scene;
class Entity
{
public:
    Entity(entt::entity id, Scene* scene);
    Entity(uint32_t id, Scene* scene);
    Entity() = default;

    // Returns and adds component of type T
    template <typename T, typename... Args>
    T& AddComponent(Args &&...args);

    // Returns true if entity succesefully removed component <T>
    template <typename T, typename... Args>
    bool RemoveComponent(Args &&...args);

    // Returns true if entity has component <T>
    template <typename T>
    bool HasComponent() const;

    // Returns component of type <T>
    template <typename T>
    T& GetComponent();

    // Tries to get component of type <T> as out
    template <typename T>
    bool TryGetComponent(T** out);

    const bool IsValid() const;

    // Returns entity handle
    const uint32_t Id() const { return static_cast<uint32_t>(m_id); }

    bool operator==(const Entity& other) const { return m_id == other.m_id; }
    bool operator!=(const Entity& other) const { return m_id != other.m_id; }

    friend std::ostream& operator<<(std::ostream& stream, Entity& e);

    operator bool() const { return IsValid();  }
    operator entt::entity() const { return m_id; }
private:
    friend class Scene;
    friend class ScriptableEntity;

    entt::entity m_id = entt::null;
    Scene* m_scene = nullptr;
};
