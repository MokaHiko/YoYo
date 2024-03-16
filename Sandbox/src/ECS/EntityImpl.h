#pragma once
#include "Scene.h"

template <typename T, typename... Args>
T& Entity::AddComponent(Args &&...args)
{
	return m_scene->AddComponent<T>(m_id, args...);
}

template <typename T, typename... Args>
bool Entity::RemoveComponent(Args &&...args)
{
	return m_scene->RemoveComponent<T>(m_id, args...);
}

template <typename T>
bool Entity::HasComponent() const
{
	return m_scene->HasComponent<T>(m_id);
}

template <typename T>
T& Entity::GetComponent()
{
	return m_scene->GetComponent<T>(m_id);
}