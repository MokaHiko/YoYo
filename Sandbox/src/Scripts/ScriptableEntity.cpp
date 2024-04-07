#include "ScriptableEntity.h"

#include <Core/Assert.h>
#include <Events/Event.h>
#include <Core/Application.h>

#include "ECS/Scene.h"
#include "NativeScript.h"
#include "Process.h"

#include "ScriptEvents.h"
#include "Physics/Physics3D.h"

ScriptableEntity::ScriptableEntity(Entity e)
	:m_entity(e)
{
	YASSERT(e, "Scriptable entity cannot be attached to invalid entity!");

	// TODO: Cache common events
	static Ref<ScriptCreatedEvent> script_created_event = CreateRef<ScriptCreatedEvent>(this);
	script_created_event->script = this;

	yoyo::EventManager::Instance().Dispatch(script_created_event);
}

ScriptableEntity::~ScriptableEntity()
{
}

Entity ScriptableEntity::GameObject()
{
	YASSERT(m_entity, "Invalid Entity Attached to script");
	return m_entity;
}

Entity ScriptableEntity::Instantiate(const std::string& name, const yoyo::Vec3& position)
{
	auto e = m_entity.m_scene->Instantiate(name, position);
	return e;
}

Entity ScriptableEntity::Instantiate(const std::string& name, const yoyo::Mat4x4& transform)
{
	auto e = m_entity.m_scene->Instantiate(name, transform);
	return e;
}

void ScriptableEntity::DestroyObject(Entity e)
{
	if (e.IsValid())
	{
		m_entity.m_scene->QueueDestroy(e);
	}
}

void ScriptableEntity::QueueDestroy()
{
	// Check if already queued
	if (m_to_destroy)
	{
		return;
	}

	m_entity.m_scene->QueueDestroy(m_entity);
	m_to_destroy = true;
}

void ScriptableEntity::Destroy()
{
	m_entity.m_scene->Destroy(m_entity);
}

bool ScriptableEntity::Raycast(const yoyo::Vec3& origin, const yoyo::Vec3& dir, float max_distance, psx::RaycastHit& out)
{
	YASSERT(m_physics_world != nullptr && "Physics world must not be null");
	return m_physics_world->Raycast(origin, dir, max_distance, out);
}

void ScriptableEntity::StartProcess(Ref<Process> process)
{
	if (!process)
	{
		return;
	}

	if (process->IsAlive())
	{
		YERROR("Process already started!");
	}

	YASSERT(m_scripting_system, "No process layer!");
	m_scripting_system->AttachProcess(process);
}