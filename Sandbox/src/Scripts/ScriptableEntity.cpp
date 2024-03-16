#include "ScriptableEntity.h"

#include <Core/Assert.h>
#include <Events/Event.h>

#include "ECS/Scene.h"
#include "NativeScript.h"

#include "ScriptEvents.h"

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


void ScriptableEntity::QueueDestroy() 
{ 
	m_entity.m_scene->QueueDestroy(m_entity);
}

// void ScriptableEntity::StartProcess(Ref<Process> process)
// {
// 	if (!process)
// 	{
// 		return;
// 	}

// 	if (process->IsAlive())
// 	{
// 		MRS_ERROR("Process already started!");
// 	}

// 	static ProcessLayer* process_layer = dynamic_cast<ProcessLayer*>(Application::Instance().FindLayer("ProcessLayer"));

// 	if (process_layer)
// 	{
// 		process_layer->AttachProcess(process);
// 	}
// 	else
// 	{
// 		MRS_ERROR("Process Layer not found!");
// 	}
// }

