#include "Input.h"
#include "InputEvent.h"

#include "Core/Log.h"
#include "Core/Assert.h"

#include "Math/Math.h"

namespace yoyo
{
	int Input::x;
	int Input::y;
	int Input::x_rel;
	int Input::y_rel;

	bool Input::LastKeys[322] = { 0 };
	bool Input::Keys[322] = { 0 };
	bool Input::LastMouseButtons[4] = { 0 };
	bool Input::MouseButtons[4] = { 0 };

	void InputLayer::OnAttach()
	{
	}

	void InputLayer::OnDetatch()
	{
	}

	void InputLayer::OnEnable()
	{
		Input::x = 0;
		Input::y = 0;

		memset(Input::LastKeys, 0, 322);
		memset(Input::Keys, 0, 322);
		memset(Input::LastMouseButtons, 0, 4);
		memset(Input::MouseButtons, 0, 4);

		EventManager::Instance().Subscribe(KeyDownEvent::s_event_type, [&](Ref<Event> event){
			return OnEvent(event);
		});

		EventManager::Instance().Subscribe(KeyUpEvent::s_event_type, [&](Ref<Event> event){
			return OnEvent(event);
		});

		EventManager::Instance().Subscribe(MouseButtonDownEvent::s_event_type, [&](Ref<Event> event){
			return OnEvent(event);
		});

		EventManager::Instance().Subscribe(MouseButtonUpEvent::s_event_type, [&](Ref<Event> event){
			return OnEvent(event);
		});

		EventManager::Instance().Subscribe(MouseMoveEvent::s_event_type, [&](Ref<Event> event){
			return OnEvent(event);
		});
	}

	void InputLayer::OnDisable()
	{
		// TODO: Unsubscribe 
	}

	bool InputLayer::OnEvent(Ref<Event> event)
	{
		if (event->Type() == KeyDownEvent::s_event_type)
		{
			Ref<KeyDownEvent> key_down_event = std::static_pointer_cast<KeyDownEvent>(event);
			KeyCode key = key_down_event->key;

			if (Input::Keys[(int)key])
			{
				Input::LastKeys[(int)key] = true;
			}

			Input::Keys[(int)key] = true;
			return true;
		}
		else if (event->Type() == KeyUpEvent::s_event_type)
		{
			const Ref<KeyUpEvent>& key_up_event = std::static_pointer_cast<KeyUpEvent>(event);

			KeyCode key = key_up_event->key;

			Input::LastKeys[(int)key] = false;
			Input::Keys[(int)key] = false;

			return true;
		}
		else if (event->Type() == MouseButtonDownEvent::s_event_type)
		{
			const Ref<MouseButtonDownEvent>& mouse_button_down_event = std::static_pointer_cast<MouseButtonDownEvent>(event);
			int mouse_button = mouse_button_down_event->button;

			if (Input::MouseButtons[(int)mouse_button])
			{
				Input::LastMouseButtons[(int)mouse_button] = true;
			}

			Input::MouseButtons[(int)mouse_button] = true;
			return true;
		}
		else if (event->Type() == MouseButtonUpEvent::s_event_type)
		{
			const Ref<MouseButtonUpEvent>& mouse_button_up_event = std::static_pointer_cast<MouseButtonUpEvent>(event);

			int mouse_button = mouse_button_up_event->button;

			Input::LastMouseButtons[mouse_button] = false;
			Input::MouseButtons[mouse_button] = false;

			return true;
		}
		else if (event->Type() == MouseMoveEvent::s_event_type)
		{
			const Ref<MouseMoveEvent>& mouse_button_up_event = std::static_pointer_cast<MouseMoveEvent>(event);

			Input::x = mouse_button_up_event->x;
			Input::y = mouse_button_up_event->y;

			Input::x_rel = mouse_button_up_event->x_rel;
			Input::y_rel = mouse_button_up_event->y_rel;

			return true;
		}

		return false;
	}

	bool Input::GetKeyDown(KeyCode key)
	{
		// TODO: Fix in event handling
		if(Input::Keys[(int)key] && !(Input::LastKeys[(int)(key)]))
		{
			Input::LastKeys[(int)(key)] = true;
			return true;
		}
		return false;
	}

    bool Input::GetMouseButton(MouseButton mouse_button)
	{
		YASSERT((int)mouse_button>= 0 && (int)mouse_button < 4);
		return Input::MouseButtons[(int)(mouse_button)];
	}

	bool Input::GetMouseButtonDown(MouseButton mouse_button)
	{
		if(Input::MouseButtons[(int)mouse_button] && !(Input::LastMouseButtons[(int)(mouse_button)]))
		{
			Input::LastMouseButtons[(int)(mouse_button)] = true;
			return true;
		}

		return false;
	}

	IVec2 Input::GetMousePosition()
	{
		return{Input::x, Input::y};
	}

	bool Input::GetKey(KeyCode key)
	{
		return Input::Keys[(int)key];
	}
}