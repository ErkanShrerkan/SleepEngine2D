#include "pch.h"
#include "Input.h"
//#include "Keyboard.h"
#include <Windows.h>
#include "GraphicsEngine.h"
#include "Engine.h"
#include <algorithm>

Keyboard Input::myKeyboard;
Vector2f Input::myMousePosition;
Vector2f Input::myMouseDelta;
std::vector<KeyUpdate> Input::myKeyUpdatesToDispatch;
std::vector<Input::InputEvent> Input::myInputEvents;
std::vector<std::vector<uint>> Input::myEventTriggers;
bool Input::myLockCursor = false;
bool Input::myInvertedY = false;
bool Input::myDragIsActive = false;
float Input::myMouseSensitivity = 1.0f;
float Input::myScrollDelta = 0.0f;

#define ET myEventTriggers

void Input::SetInvertedY(bool aState)
{
	myInvertedY = aState;
}

bool Input::GetInvertedY()
{
	return myInvertedY;
}

Input::Input()
{
}

Input::~Input()
{
	DeInit();
}

void Input::RegisterInputEvent(InputEvent anIE)
{
}

void Input::DeInit()
{
	//for (auto& [key, eventMap] : myObservedInputEvents)
	//{
	//	eventMap.clear();
	//}

	//myObservedInputEvents.clear();
	myInputEvents.clear();
	myEventTriggers.clear();
}

void Input::Init()
{
	//typedef InputKey ;
	//RegisterInputEvent{ eInputEvent::ToggleAllPasses, { VK_F1 },  }
	myEventTriggers.resize((uint)eInputEvent::COUNT);
	ET[(uint)eInputEvent::ToggleAllPasses] = { VK_F1 };
	ET[(uint)eInputEvent::TogglePositionPass] = { VK_F2 };
	ET[(uint)eInputEvent::ToggleAlbedoPass] = { VK_F3 };
	ET[(uint)eInputEvent::ToggleNormalPass] = { VK_F4 };
	ET[(uint)eInputEvent::ToggleVertexNormalPass] = { VK_F5 };
	ET[(uint)eInputEvent::ToggleMaterialPass] = { VK_F6 };
	ET[(uint)eInputEvent::ToggleAOPass] = { VK_F7 };
	ET[(uint)eInputEvent::ToggleDepthPass] = { VK_F8 };

	ET[(uint)eInputEvent::LMB] = { VK_LBUTTON };
	ET[(uint)eInputEvent::RMB] = { VK_RBUTTON };
	ET[(uint)eInputEvent::Escape] = { VK_ESCAPE };
	ET[(uint)eInputEvent::Jump] = { VK_SPACE };
	ET[(uint)eInputEvent::MovementToggle] = { VK_SHIFT };

	ET[(uint)eInputEvent::RotUp] = { VK_UP };
	ET[(uint)eInputEvent::RotDown] = { VK_DOWN };
	ET[(uint)eInputEvent::RotLeft] = { VK_LEFT };
	ET[(uint)eInputEvent::RotRight] = { VK_RIGHT };

	ET[(uint)eInputEvent::Up] = { 'W' };
	ET[(uint)eInputEvent::Down] = { 'S' };
	ET[(uint)eInputEvent::Forward] = { 'W' };
	ET[(uint)eInputEvent::Back] = { 'S' };
	ET[(uint)eInputEvent::Left] = { 'A' };
	ET[(uint)eInputEvent::Right] = { 'D' };

	ET[(uint)eInputEvent::ChangeCamera] = { 'Y' };
	ET[(uint)eInputEvent::ToggleDrawLine] = { 'Z' };

	ET[(uint)eInputEvent::H] = { 'H' };
	ET[(uint)eInputEvent::J] = { 'J' };
	ET[(uint)eInputEvent::G] = { 'G' };
	ET[(uint)eInputEvent::Interact] = { 'F' };
	ET[(uint)eInputEvent::UseItem] = { 'E' };
	ET[(uint)eInputEvent::Reload] = { 'R' };

	ET[(uint)eInputEvent::O] = { 'O' };
	ET[(uint)eInputEvent::P] = { 'P' };
	ET[(uint)eInputEvent::Quit] = { 'I' };
	ET[(uint)eInputEvent::Alt] = { VK_MENU };
	ET[(uint)eInputEvent::F4] = { VK_F4 };

	ET[(uint)eInputEvent::One] = { '1' };
	ET[(uint)eInputEvent::Two] = { '2' };
	ET[(uint)eInputEvent::Three] = { '3' };

	// add registered keys to keyboard
	for (auto& keys : ET)
	{
		for (auto key : keys)
		{
			myKeyboard.AddKey(key);
		}
	}
}

bool Input::GetInputPressed(eInputEvent anInput)
{
	bool pc = false;

	for (auto& input : myEventTriggers[(uint)anInput])
	{
		pc = myKeyboard.GetKeyPressed(input);
		if (pc)
		{
			break;
		}
	}
	return pc;
}

bool Input::GetInputHeld(eInputEvent anInput)
{
	bool pc = false;
	for (auto& input : myEventTriggers[(uint)anInput])
	{
		pc = myKeyboard.GetKeyHeld(input);
		if (pc)
		{
			break;
		}
	}
	return pc;
}

bool Input::GetInputReleased(eInputEvent anInput)
{
	bool pc = false;
	for (auto& input : myEventTriggers[(uint)anInput])
	{
		pc = myKeyboard.GetKeyReleased(input);
		if (pc)
		{
			break;
		}
	}
	return pc;
}

bool Input::GetInputDown(eInputEvent anInput)
{
	bool pc = false;
	for (auto& input : myEventTriggers[(uint)anInput])
	{
		pc = myKeyboard.GetKeyDown(input);
		if (pc)
		{
			break;
		}
	}
	return pc;
}

Vector2f Input::GetMousePos()
{
	POINT p;
	GetCursorPos(&p);
	RECT rect = {};
	HWND handle = GetActiveWindow();
	GetWindowRect(handle, &rect);
	ScreenToClient(handle, &p);
	return Vector2f{ (float)p.x / (rect.right - rect.left), (float)p.y / (rect.bottom - rect.top) };
}

Vector2f Input::GetClampedMousePos()
{
	POINT p;
	GetCursorPos(&p);
	RECT rect = {};
	HWND handle = GetActiveWindow();
	GetWindowRect(handle, &rect);
	ScreenToClient(handle, &p);
	Vector2f screenPos = { (float)p.x / (rect.right - rect.left), (float)p.y / (rect.bottom - rect.top) };
	return Vector2f{ std::clamp(screenPos.x,0.0f,1.0f),std::clamp(screenPos.y,0.0f,1.0f) };
}

Vector2f Input::GetMouseDelta()
{
	if (myLockCursor)
	{
		return myMouseDelta;
	}
	else
	{
		return { 0,0 };
	}
}

void Input::Update(bool doUpdate)
{
	myKeyUpdatesToDispatch.clear();

	if (!doUpdate)
	{
		myMouseDelta = { 0, 0 };
		return;
	}

	myKeyUpdatesToDispatch = myKeyboard.Update();
	POINT p;
	HWND handle = GetActiveWindow();
	GetCursorPos(&p);
	ScreenToClient(handle, &p);

	myMouseDelta.x = p.x - myMousePosition.x;
	myMouseDelta.y = p.y - myMousePosition.y;
	if (myInvertedY)
		myMouseDelta.y *= -1.0f;
	myMouseDelta *= myMouseSensitivity * (myDragIsActive ? .5f : 1.f);

	if (myLockCursor)
	{
		myMousePosition.x = 900;
		myMousePosition.y = 600;

		SetCursorPos(900, 600);
	}
	else
	{
		myMousePosition.x = static_cast<float>(p.x);
		myMousePosition.y = static_cast<float>(p.y);
	}

	uint loops = 0;
	for (auto& keyUpdate : myKeyUpdatesToDispatch)
	{
		uint uKey = keyUpdate.key;
		eInputState uState = keyUpdate.state;

		for (auto& inputEvent : myInputEvents)
		{
			if ((inputEvent.state & uState) == eInputState::Null)
				continue;

			bool eventTriggered = false;
			for (auto& key : myEventTriggers[(uint)inputEvent.event])
			{
				if (key != uKey)
					continue;

				eventTriggered = true;
				break;
			}

			if (eventTriggered)
			{
				for (auto& observerCallback : inputEvent.callbacks)
				{
					observerCallback.callback();
					loops++;
				}
			}
		}
	}
}

void Input::AddInputEventObserver(InputObserver* anObserver, eInputEvent anEvent, eInputState aState, std::function<void()>& aCallback)
{
	InputEvent check;
	check.event = anEvent;
	check.state = aState;

	bool handled = false;
	for (auto& inputEvent : myInputEvents)
	{
		if (inputEvent.IsEqual(check))
		{
			inputEvent.callbacks.push_back({ anObserver, aCallback });
			handled = true;
		}
	}

	if (!handled)
	{
		check.callbacks.push_back({ anObserver, aCallback });
		myInputEvents.push_back(check);
	}
}

void Input::RemoveEventObserver(InputObserver* anObserver, eInputEvent anEvent, eInputState aState)
{
	InputEvent check;
	check.event = anEvent;
	check.state = aState;

	bool handled = false;
	uint index = 0;
	for (auto& inputEvent : myInputEvents)
	{
		if (!inputEvent.IsEqual(check))
			continue;

		for (uint i = 0; i < inputEvent.callbacks.size(); i++)
		{
			if (inputEvent.callbacks[i].observer != anObserver)
				continue;

			handled = true;
			index = i;
			break;
		}

		if (handled)
		{
			inputEvent.callbacks.erase(inputEvent.callbacks.begin() + index);
		}

		break;
	}
}

void Input::RemoveEventObserver(InputObserver* anObserver)
{
	for (auto& inputEvent : myInputEvents)
	{
		printf("--------------------------------\n");
		printf("Event callbacks: %i\n", (int)inputEvent.callbacks.size());
		inputEvent.callbacks.erase(
			std::remove_if(inputEvent.callbacks.begin(), inputEvent.callbacks.end(),
				[&](ObserverCallback& aCallback)
				{
					return aCallback.observer == anObserver;
				}),
			inputEvent.callbacks.end());
		printf("Event callbacks after: %i\n", (int)inputEvent.callbacks.size());
	}
}

void Input::LockCursor(bool aShouldLock)
{
	myLockCursor = aShouldLock;
}

float Input::GetMouseSensitivity()
{
	return myMouseSensitivity;
}
void Input::SetMouseSensitivity(float aSensitity)
{
	myMouseSensitivity = aSensitity;
}
void Input::ScrollEvent(MSG aMessage)
{
	myScrollDelta = GET_WHEEL_DELTA_WPARAM(aMessage.wParam);
}

int Input::GetScrollInput()
{
	if (myScrollDelta > 0)
	{
		return -1;
	}
	else if (myScrollDelta < 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void InputObserver::ObserveInputEvent(eInputEvent anEvent, eInputState aTriggerState, std::function<void()> aCallback)
{
	Input::AddInputEventObserver(this, anEvent, aTriggerState, aCallback);
}

void InputObserver::StopObservingInputEvent(eInputEvent anEvent, eInputState aTriggerState)
{
	Input::RemoveEventObserver(this, anEvent, aTriggerState);
}

InputObserver::~InputObserver()
{
	Input::RemoveEventObserver(this);
}
