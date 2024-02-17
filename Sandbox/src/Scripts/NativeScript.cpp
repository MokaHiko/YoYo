#include "NativeScript.h"

ScriptableEntity::ScriptableEntity(Entity e)
	:m_entity(e) {}

ScriptableEntity::~ScriptableEntity()
{

}

Entity ScriptableEntity::Instantiate(const std::string& name)
{
	auto e = m_entity.m_scene->Instantiate(name);
	return e;
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