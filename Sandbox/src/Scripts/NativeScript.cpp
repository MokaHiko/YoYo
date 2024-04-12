#include "NativeScript.h"

#include "ScriptEvents.h"
#include "Process.h"

#include "Physics/PhysicsEvents.h"

void ScriptingSystem::Init()
{
	yoyo::EventManager::Instance().Subscribe(ScriptCreatedEvent::s_event_type, [&](Ref<yoyo::Event> event) {
		const Ref<ScriptCreatedEvent>& script_event = std::static_pointer_cast<ScriptCreatedEvent>(event);
		OnScriptCreatedCallback(script_event->script);
		return false;
	});

	yoyo::EventManager::Instance().Subscribe(ScriptDestroyedEvent::s_event_type, [&](Ref<yoyo::Event> event) {
		const Ref<ScriptDestroyedEvent>& script_event = std::static_pointer_cast<ScriptDestroyedEvent>(event);
		OnScriptDestroyedCallback(script_event->script);
		return false;
	});

	yoyo::EventManager::Instance().Subscribe(psx::CollisionEvent::s_event_type, [&](Ref<yoyo::Event> event) {
		const Ref<psx::CollisionEvent>& col_event = std::static_pointer_cast<psx::CollisionEvent>(event);
		OnCollisionCallback(col_event->collision);
		return false;
	});
}

void ScriptingSystem::Shutdown()
{
	for (ScriptableEntity* script : m_script_cache)
	{
		YDELETE script;
	}
}

void ScriptingSystem::Update(float dt)
{
	for (auto entity : GetScene()->Registry().view<TransformComponent, NativeScriptComponent>())
	{
		Entity e(entity, GetScene());
		NativeScriptComponent& ns = e.GetComponent<NativeScriptComponent>();

		for (int i = 0; i < ns.m_scripts_count; i++)
		{
			if (!ns.m_scripts[i]->IsActive())
			{
				continue;
			}

			if (!ns.m_scripts[i]->started)
			{
				ns.m_scripts[i]->OnStart();
				ns.m_scripts[i]->started = true;
			}

			ns.m_scripts[i]->OnUpdate(dt);
		}
	}

	// Process are updated after all scripts
	UpdateProcesses(dt);
}

void ScriptingSystem::OnComponentCreated(Entity e, NativeScriptComponent& native_script)
{
}

void ScriptingSystem::OnComponentDestroyed(Entity e, NativeScriptComponent& native_script)
{
	for (int i = 0; i < native_script.m_scripts_count; i++)
	{
		native_script.RemoveScript(native_script.m_scripts[i]);
	}
}

void ScriptingSystem::OnScriptCreatedCallback(ScriptableEntity* script)
{
	if (!script->GameObject().HasComponent<NativeScriptComponent>())
	{
		auto& ns = script->GameObject().AddComponent<NativeScriptComponent>();
		ns.AddScript(script);
	}
	else
	{
		NativeScriptComponent& ns = script->GameObject().GetComponent<NativeScriptComponent>();
		ns.AddScript(script);
	}

	// TODO: Find a better way to handle this, possibly event driven.
	// Assign scripting system and physics world
	script->m_scripting_system = this;
	script->m_physics_world = m_physics_world;
}

void ScriptingSystem::OnScriptDestroyedCallback(ScriptableEntity* script)
{
	YASSERT(script, "attempting to destroy invalid script");

	if (!script->GameObject().HasComponent<NativeScriptComponent>())
	{
		auto& ns = script->GameObject().AddComponent<NativeScriptComponent>();
		ns.RemoveScript(script);

		script->OnDestroy();
		YDELETE script;
	}
}

void ScriptingSystem::OnCollisionCallback(psx::Collision& col)
{
	Entity e1(col.a, GetScene());
	Entity e2(col.b, GetScene());

	if (e1.HasComponent<NativeScriptComponent>())
	{
		NativeScriptComponent& ns = e1.GetComponent<NativeScriptComponent>();

		for (int i = 0; i < ns.m_scripts_count; i++)
		{
			if (!ns.m_scripts[i]->IsActive())
			{
				continue;
			}

			ns.m_scripts[i]->OnCollisionEnter(col);
		}
	}

	if (e2.HasComponent<NativeScriptComponent>())
	{
		NativeScriptComponent& ns = e2.GetComponent<NativeScriptComponent>();
		std::swap(col.a, col.b);

		for (int i = 0; i < ns.m_scripts_count; i++)
		{
			if (!ns.m_scripts[i]->IsActive())
			{
				continue;
			}

			ns.m_scripts[i]->OnCollisionEnter(col);
		}
	}
}

void ScriptingSystem::AttachProcess(Ref<Process> process)
{
	YASSERT(process != nullptr, "Process is null");
	m_processes.push_back(process);
}

void ScriptingSystem::AbortAllProcesses(bool immediate)
{
	// TODO: Abort all processes
}

void ScriptingSystem::UpdateProcesses(float dt)
{
	auto it = m_processes.begin();

	uint8_t success_count = 0;
	uint8_t fail_count = 0;

	while (it != m_processes.end()) {
		Ref<Process> process = (*it);

		std::list<Ref<Process>>::iterator this_it = it;
		it++;

		if (process->State() == Process::Uninitialized)
		{
			process->OnInit();
		}

		if (process->State() == Process::Running)
		{
			process->OnUpdate(dt);
		}

		if (process->IsDead()) {
			switch (process->State())
			{
			case Process::ProcessState::Succeeded:
			{
				process->OnSuccess();
				if (process->Child())
				{
					AttachProcess(process->Child());
				}
				else
				{
					success_count++;
				}
			} break;
			case Process::ProcessState::Failed:
			{
				process->OnFail();
				fail_count++;
			} break;
			case Process::ProcessState::Aborted:
			{
				process->OnAbort();
				fail_count++;
			} break;
			}

			m_processes.erase(this_it);
		}
	}
}

NativeScriptComponent::NativeScriptComponent() {}

void NativeScriptComponent::AddScript(ScriptableEntity* script)
{
	YASSERT(script != nullptr, "Cannot add invalid script!");
	auto it = std::find(m_scripts.begin(), m_scripts.end(), script);

	if (it != m_scripts.end())
	{
		YERROR("Entity already has script!");
	}
	else
	{
		YASSERT(m_insert_index < MAX_SCRIPTS, "Maximum scripts on entity reached!");
		script->ToggleActive(true);

		m_scripts[m_insert_index++] = script;
		m_scripts_count++;
	}
}

void NativeScriptComponent::RemoveScript(ScriptableEntity* script)
{
	//TODO: Add remove script flag
	auto it = std::find(m_scripts.begin(), m_scripts.end(), script);

	int index = -1;
	if (it != m_scripts.end())
	{
		int index = std::distance(m_scripts.begin(), it);
		m_scripts[index] = nullptr;
	}

	// Return if no script found
	if (index < 0)
	{
		return;
	}

	// Make sure scripts array is contiguos
	if (index != m_scripts.size() - 1)
	{
		m_scripts[index] = m_scripts.back();
		m_scripts[m_scripts.size() - 1] = nullptr;
		--m_scripts_count;
	}
}
