#include "Event.h"

#include "Core/Log.h"

namespace yoyo
{
	void EventManager::Init()
	{
	}
	void EventManager::Shutdown()
	{
	}

	void EventManager::Subscribe(const EventType& type, const EventHandler& handler)
	{
		auto it = m_event_listeners[type];
		// if(std::find(it.begin(), it.end(), handler) != it.end())
		// {
		// 	YINFO("EventHandler of type %d already registerd!");
		// }
		// else
		// {
		// }
		m_event_listeners[type].push_back(std::move(handler));
	}

	void EventManager::Unsubscribe(const EventType& type, EventHandler& handler)
	{
		// TODO: unsub
	}

	void EventManager::Dispatch(Ref<Event> event)
	{
		for(EventHandler& event_handler : m_event_listeners[event->Type()])
		{
			if(event_handler(event))
			{
				return;
			}
		}
	}
}