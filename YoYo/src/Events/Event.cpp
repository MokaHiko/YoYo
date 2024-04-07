#include "Event.h"

#include "Core/Log.h"

namespace yoyo
{
	// TODO: Return handler
	void EventManager::Subscribe(const EventType& type, const EventHandler& handler)
	{
		auto& it = m_event_listeners[type];

		// TODO: Change and return EventHandler to struct with id
		//auto some_thing = std::find(it.begin(), it.end(), handler);
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