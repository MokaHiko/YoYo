#include "ScriptEvents.h"

ScriptCreatedEvent::ScriptCreatedEvent(ScriptableEntity* scriptable)
	:script(scriptable)
{
}

ScriptDestroyedEvent::ScriptDestroyedEvent(ScriptableEntity* scriptable)
	:script(scriptable)
{
}
