#include "ApplicationEvent.h"

#include "Core/Time.h"
#include "Platform/Platform.h"

namespace yoyo
{
	ApplicationStartEvent::ApplicationStartEvent()
	{
		app_start_time = Time::CurrentTime();
	}

	ApplicationResizeEvent::ApplicationResizeEvent(float x, float y, float width, float height)
	{
	}

	ApplicationCloseEvent::ApplicationCloseEvent()
	{
	}
}