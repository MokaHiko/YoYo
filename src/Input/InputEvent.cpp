#include "InputEvent.h"

#include "Platform/Platform.h"

namespace yoyo
{
	KeyDownEvent::KeyDownEvent(KeyCode key_pressed)
		:key(key_pressed) {}


	KeyUpEvent::KeyUpEvent(KeyCode key_released)
		:key(key_released) {}

	MouseButtonUpEvent::MouseButtonUpEvent(int index)
		:button(index) {}

	MouseButtonDownEvent::MouseButtonDownEvent(int index)
		:button(index) {}
};