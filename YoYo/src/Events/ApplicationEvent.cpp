#include "ApplicationEvent.h"

#include "Core/Time.h"
#include "Platform/Platform.h"

namespace yoyo
{
    const EventType ApplicationStartEvent::s_event_type = Platform::GenerateUUIDV4();
	ApplicationStartEvent::ApplicationStartEvent()
	{
		app_start_time = Time::CurrentTime();
	}

    const EventType ApplicationResizeEvent::s_event_type = Platform::GenerateUUIDV4();
	ApplicationResizeEvent::ApplicationResizeEvent(float x, float y, float width, float height)
	{
	}

    const EventType ApplicationCloseEvent::s_event_type = Platform::GenerateUUIDV4();
	ApplicationCloseEvent::ApplicationCloseEvent()
	{
	}
}