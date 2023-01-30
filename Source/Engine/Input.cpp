#include "pch.h"
#include "Input.h"
#include "Keyboard.h"
#include <Windows.h>
#include "GraphicsEngine.h"
#include "Engine.h"
#include <algorithm>

Keyboard Input::myKeyboard;
Vector2f Input::myMousePosition;
Vector2f Input::myMouseDelta;
std::unordered_map<eButtonInput, Input::InputValue> Input::myButtonInputs;
bool Input::myLockCursor = false;
bool Input::myInvertedY = false;
bool Input::myDragIsActive = false;
float Input::myMouseSensitivity = 1.0f;
float Input::myScrollDelta = 0.0f;

#define BI myButtonInputs

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

void Input::Init()
{
	typedef InputValue IV;
	BI[eButtonInput::ToggleAllPasses] = IV({ VK_F1 });
	BI[eButtonInput::TogglePositionPass] = IV({ VK_F2 });
	BI[eButtonInput::ToggleAlbedoPass] = IV({ VK_F3 });
	BI[eButtonInput::ToggleNormalPass] = IV({ VK_F4 });
	BI[eButtonInput::ToggleVertexNormalPass] = IV({ VK_F5 });
	BI[eButtonInput::ToggleMaterialPass] = IV({ VK_F6 });
	BI[eButtonInput::ToggleAOPass] = IV({ VK_F7 });
	BI[eButtonInput::ToggleDepthPass] = IV({ VK_F8 });

	BI[eButtonInput::LMB] = IV({ VK_LBUTTON });
	BI[eButtonInput::RMB] = IV({ VK_RBUTTON });
	BI[eButtonInput::Escape] = IV({ VK_ESCAPE });
	BI[eButtonInput::Jump] = IV({ VK_SPACE });
	BI[eButtonInput::MovementToggle] = IV({ VK_SHIFT });

	BI[eButtonInput::RotUp] = IV({ VK_UP });
	BI[eButtonInput::RotDown] = IV({ VK_DOWN });
	BI[eButtonInput::RotLeft] = IV({ VK_LEFT });
	BI[eButtonInput::RotRight] = IV({ VK_RIGHT });

	BI[eButtonInput::Up] = IV({ 'W' });
	BI[eButtonInput::Down] = IV({ 'S' });
	BI[eButtonInput::Forward] = IV({ 'W' });
	BI[eButtonInput::Back] = IV({ 'S' });
	BI[eButtonInput::Left] = IV({ 'A' });
	BI[eButtonInput::Right] = IV({ 'D' });

	BI[eButtonInput::ChangeCamera] = IV({ 'Y' });
	BI[eButtonInput::ToggleDrawLine] = IV({ 'Z' });


	BI[eButtonInput::H] = IV({ 'H' });
	BI[eButtonInput::J] = IV({ 'J' });
	BI[eButtonInput::G] = IV({ 'G' });
	BI[eButtonInput::Interact] = IV({ 'F' });
	BI[eButtonInput::UseItem] = IV({ 'E' });
	BI[eButtonInput::Reload] = IV({ 'R' });

	BI[eButtonInput::O] = IV({ 'O' });
	BI[eButtonInput::P] = IV({ 'P' });
	BI[eButtonInput::Quit] = IV({ 'I' });
	BI[eButtonInput::Alt] = IV({ VK_MENU });
	BI[eButtonInput::F4] = IV({ VK_F4 });

	BI[eButtonInput::One] = IV({ '1' });
	BI[eButtonInput::Two] = IV({ '2' });
	BI[eButtonInput::Three] = IV({ '3' });

}

bool Input::GetInputPressed(eButtonInput anInput)
{
	bool pc = false;

	for (auto& input : myButtonInputs[anInput].myKeyInput)
	{
		pc = myKeyboard.GetKeyPressed(input);
		if (pc)
		{
			break;
		}
	}
	return pc;
}

bool Input::GetInputHeld(eButtonInput anInput)
{
	bool pc = false;
	for (auto& input : myButtonInputs[anInput].myKeyInput)
	{
		pc = myKeyboard.GetKeyHeld(input);
		if (pc)
		{
			break;
		}
	}
	return pc;
}

bool Input::GetInputReleased(eButtonInput anInput)
{
	bool pc = false;
	for (auto& input : myButtonInputs[anInput].myKeyInput)
	{
		pc = myKeyboard.GetKeyReleased(input);
		if (pc)
		{
			break;
		}
	}
	return pc;
}

bool Input::GetInputDown(eButtonInput anInput)
{
	bool pc = false;
	for (auto& input : myButtonInputs[anInput].myKeyInput)
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
	return Vector2f({ (float)p.x / (rect.right - rect.left), (float)p.y / (rect.bottom - rect.top) });
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
	return Vector2f({ std::clamp(screenPos.x,0.0f,1.0f),std::clamp(screenPos.y,0.0f,1.0f) });
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

void Input::Update()
{
	myKeyboard.Update();
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

}

void Input::LockCursor(bool aShouldLock)
{
	myLockCursor = aShouldLock;
}

float Input::GetMouseSensitivity()
{
	return myMouseSensitivity;
}
void Input::SetMouseSensitivity(float aSensitivity)
{
	myMouseSensitivity = aSensitivity;
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

void Input::AddKey(unsigned int aKey)
{
	myKeyboard.AddKey(aKey);
}
