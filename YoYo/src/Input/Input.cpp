#include "Input.h"
#include "InputEvent.h"

#include "Core/Log.h"
#include "Core/Assert.h"

namespace yoyo
{
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
			Ref<KeyUpEvent> key_up_event = std::static_pointer_cast<KeyUpEvent>(event);

			KeyCode key = key_up_event->key;

			Input::LastKeys[(int)key] = false;
			Input::Keys[(int)key] = false;

			return true;
		}
		else if (event->Type() == MouseButtonDownEvent::s_event_type)
		{
			Ref<MouseButtonDownEvent> mouse_button_down_event = std::static_pointer_cast<MouseButtonDownEvent>(event);
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
			Ref<MouseButtonUpEvent> mouse_button_up_event = std::static_pointer_cast<MouseButtonUpEvent>(event);

			int mouse_button = mouse_button_up_event->button;

			Input::LastMouseButtons[mouse_button] = false;
			Input::MouseButtons[mouse_button] = false;

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

	bool Input::GetMouseButton(int index)
	{
		YASSERT(index > 0 && index < 4);
		return Input::MouseButtons[index];
	}

	bool Input::GetMouseButtonDown(KeyCode key)
	{
		if(Input::MouseButtons[(int)key] && !(Input::LastMouseButtons[(int)(key)]))
		{
			Input::LastMouseButtons[(int)(key)] = true;
			return true;
		}

		return false;
	}

	bool Input::GetKey(KeyCode key)
	{
		return Input::Keys[(int)key];
	}
}