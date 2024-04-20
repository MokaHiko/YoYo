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

	MouseMoveEvent::MouseMoveEvent(int _x, int _y, int _x_rel, int _y_rel)
		:x(_x), y(_y), x_rel(_x_rel), y_rel(_y_rel) {}
};