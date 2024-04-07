#include "Process.h"

Process::Process() {}

Process::~Process() {}

Ref<Process> Process::RemoveChild()
{
	if (m_child_process)
	{
		Ref<Process> popped = m_child_process;
		m_child_process = nullptr;

		return popped;
	}

	return nullptr;
}

Ref<Process> Process::Child()
{
	return m_child_process;
}

DelayProcess::DelayProcess(float time, std::function<void(void)> callback)
{
	m_time = time;
	m_callback = callback;
}

void DelayProcess::OnUpdate(float dt)
{
	m_time -= dt;
	if (m_time <= 0)
	{
		if (m_callback)
		{
			m_callback();
		}
		Succeed();
	}
};