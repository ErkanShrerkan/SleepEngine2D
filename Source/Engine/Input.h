#pragma once
#include <vector>
#include <unordered_map>
#include "pch.h"
#include <CommonUtilities/Math/Vector.hpp>
#include <windows.h>

class Keyboard;

enum class eButtonInput
{
	Up,
	Down,
	Forward,
	Back,
	Left,
	Right,
	Escape,
	Jump,
	ToggleAllPasses,
	TogglePositionPass,
	ToggleAlbedoPass,
	ToggleNormalPass,
	ToggleVertexNormalPass,
	ToggleMaterialPass,
	ToggleAOPass,
	ToggleDepthPass,
	MovementToggle,
	RotUp,
	RotDown,
	RotLeft,
	RotRight,
	LMB,
	RMB,
	ChangeCamera,
	ToggleDrawLine,
	H,
	G,
	J,
	O,
	P,
	Interact,
	UseItem,
	Reload,
	One,
	Two,
	Three,
	Quit,
	Alt,
	F4,
};

class Input
{
	friend class InputValue;
public:
	static bool GetInputPressed(eButtonInput anInput);
	static bool GetInputHeld(eButtonInput anInput);
	static bool GetInputReleased(eButtonInput anInput);
	static bool GetInputDown(eButtonInput anInput);
	static float2 GetMousePos();
	static Vector2f GetMouseDelta();
	static float2 GetClampedMousePos();
	static int GetScrollInput();

	static void Init();
	static void Update();
	static void LockCursor(bool aShouldLock = true);
	static bool GetLockedCursorState() { return myLockCursor; }
	static float GetMouseSensitivity();
	static void SetMouseSensitivity(float aSensitivity);
	static void ScrollEvent(MSG aMessage);
	static void ResetScrollInput() { myScrollDelta = 0; }

	Input(Input const&) = delete;
	void operator=(Input const&) = delete;

	static void SetInvertedY(bool aState);
	static bool GetInvertedY();
	static void SetDrag(bool aDrag) { myDragIsActive = aDrag; }

private:
	Input();
	static void AddKey(unsigned int aKey);

	static Keyboard myKeyboard;
	struct InputValue
	{
		std::vector<unsigned int> myKeyInput;

		InputValue(std::vector<unsigned int> aKeyInput)
			: myKeyInput(aKeyInput)
		{
			for (auto key : aKeyInput)
			{
				AddKey(key);
			}
		}

		InputValue() : myKeyInput({ 0 }) {}

		bool operator<(const InputValue& aRHS) const noexcept
		{
			return this->myKeyInput[0] < aRHS.myKeyInput[0];
		}
	};
	static Vector2f myMousePosition;
	static Vector2f myMouseDelta;
	static std::unordered_map<eButtonInput, InputValue> myButtonInputs;
	static bool myLockCursor;
	static bool myInvertedY;
	static bool myDragIsActive;
	static float myMouseSensitivity;
	static float myScrollDelta;
};