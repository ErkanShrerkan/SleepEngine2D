#pragma once
#include <vector>
#include <unordered_map>
#include "pch.h"
#include <CommonUtilities/Math/Vector.hpp>
#include <windows.h>
#include "Keyboard.h"
#include <functional>

class InputObserver
{
public:
	~InputObserver();
	bool myIsObservingEditorInputs = false;

protected:
	void ObserveInputEvent(eInputEvent anEvent, eInputState aTriggerState, std::function<void()> aCallback);
	void StopObservingInputEvent(eInputEvent anEvent, eInputState aTriggerState);
};

class Input
{
	friend class InputKey;
	friend class InputEvent;
public:
	static bool GetInputPressed(eInputEvent anInput);
	static bool GetInputHeld(eInputEvent anInput);
	static bool GetInputReleased(eInputEvent anInput);
	static bool GetInputDown(eInputEvent anInput);
	static float2 GetMousePos();
	static Vector2f GetMouseDelta();
	static float2 GetClampedMousePos();
	static int GetScrollInput();

	static void DeInit();
	static void Init();
	static void Update(bool doUpdate);
	static void SetIsEditing(bool aBool) { myIsEditing = aBool; }
	/// <summary>
	/// Adds InputObserver to Input system. 
	/// TODO: assert if observer is duplicate.
	/// </summary>
	/// <param name="anObserver"></param>
	/// <param name="anEvent"></param>
	/// <param name="aState"></param>
	/// <param name="aCallback"></param>
	static void AddInputEventObserver(InputObserver* anObserver, eInputEvent anEvent, eInputState aState, std::function<void()>& aCallback);
	static void RemoveEventObserver(InputObserver* anObserver, eInputEvent anEvent, eInputState aState);
	static void RemoveEventObserver(InputObserver* anObserver);
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
	static Keyboard myKeyboard;
	struct InputKey
	{
		std::vector<uint> myKeyInput;

		InputKey(std::vector<uint> aKeyInput)
			: myKeyInput(aKeyInput)
		{
		}

		InputKey() : myKeyInput({ 0 }) {}

		bool operator<(const InputKey& aRHS) const noexcept
		{
			return this->myKeyInput[0] < aRHS.myKeyInput[0];
		}
	};

	struct ObserverCallback
	{
		InputObserver* observer;
		std::function<void()> callback;
	};

	struct InputEvent
	{
		eInputEvent event;
		eInputState state;
		std::vector<ObserverCallback> callbacks;

		bool IsEqual(InputEvent& anEvent)
		{
			return (anEvent.event == event && anEvent.state == state);
		}
	};

	static void RegisterInputEvent(InputEvent anIE);

private:
	Input();
	~Input();

private:
	static Vector2f myMousePosition;
	static Vector2f myMouseDelta;
	static std::vector<KeyUpdate> myKeyUpdatesToDispatch;
	static std::vector<InputEvent> myInputEvents;
	// event is index for myEventTriggers
	static std::vector<std::vector<uint>> myEventTriggers;
	static bool myLockCursor;
	static bool myInvertedY;
	static bool myDragIsActive;
	static bool myIsEditing;
	static float myMouseSensitivity;
	static float myScrollDelta;
};
