#pragma once

#include <Core/Assert.h>
#include "Scene.h"

template <typename T, typename... Args>
T& Entity::AddComponent(Args &&...args)
{
	YASSERT(m_scene != nullptr, "Entity does not belong to a scene!");
	return m_scene->AddComponent<T>(m_id, args...);
}

template <typename T, typename... Args>
bool Entity::RemoveComponent(Args &&...args)
{
	YASSERT(m_scene != nullptr, "Entity does not belong to a scene!");
	return m_scene->RemoveComponent<T>(m_id, args...);
}

template <typename T>
bool Entity::HasComponent() const
{
	YASSERT(m_scene != nullptr, "Entity does not belong to a scene!");
	return m_scene->HasComponent<T>(m_id);
}

template <typename T>
T& Entity::GetComponent()
{
	YASSERT(m_scene != nullptr, "Entity does not belong to a scene!");
	return m_scene->GetComponent<T>(m_id);
}

template <typename T>
bool Entity::TryGetComponent(T** out)
{
	if(!HasComponent<T>())
	{
		out = nullptr;
		return false;
	}

	*out = &GetComponent<T>();
	return true;
}