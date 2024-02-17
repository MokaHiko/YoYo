#include "Input.h"
#include "InputEvent.h"

#include "Core/Log.h"

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

	bool Input::GetKey(KeyCode key)
	{
		return Input::Keys[(int)key];
	}
}