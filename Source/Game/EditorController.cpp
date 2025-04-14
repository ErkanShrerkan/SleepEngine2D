#include "pch.h"
#include "EditorController.h"
#include "Entity.h"
#include "CameraComponent.h"
#include "EntityPickingComponent.h"
#include "Transform.h"
#include "Globals.h"

EditorController::~EditorController()
{
	myInputObserver.StopObservingAllEvents();
}

void EditorController::Start()
{
	myInputObserver.myIsObservingEditorInputs = true;
	eInputState ph = eInputState::Pressed | eInputState::Held;

	// movement
	myInputObserver.ObserveInputEvent(eInputEvent::Forward, ph, [&]() { MoveInput({0, 0, 1}); });
	myInputObserver.ObserveInputEvent(eInputEvent::Back, ph, [&]() { MoveInput({0, 0, -1}); });
	myInputObserver.ObserveInputEvent(eInputEvent::Left, ph, [&]() { MoveInput({-1, 0, 0}); });
	myInputObserver.ObserveInputEvent(eInputEvent::Right, ph, [&]() { MoveInput({1, 0, 0}); });
	myInputObserver.ObserveInputEvent(eInputEvent::E, ph, [&]() { MoveInput({ 0, 1, 0 }); });
	myInputObserver.ObserveInputEvent(eInputEvent::Q, ph, [&]() { MoveInput({ 0, -1, 0 }); });
	myInputObserver.ObserveInputEvent(eInputEvent::Space, ph, [&]() { MoveY(1); });
	myInputObserver.ObserveInputEvent(eInputEvent::Control, ph, [&]() { MoveY(-1); });
	myInputObserver.ObserveInputEvent(eInputEvent::Shift, eInputState::Pressed, [&]() { ToggleShift(true); });
	myInputObserver.ObserveInputEvent(eInputEvent::Shift, eInputState::Released, [&]() { ToggleShift(false); });

	// camera
	myInputObserver.ObserveInputEvent(eInputEvent::RMB, ph, [&]() { SetCanLook(); });
	myInputObserver.ObserveScrollEvent(eScrollState::Up, [&]() { Zoom(-1); });
	myInputObserver.ObserveScrollEvent(eScrollState::Down, [&]() { Zoom(1); });

	myInputObserver.ObserveInputEvent(eInputEvent::LMB, eInputState::Pressed, [&]() { Pick(); });

	myCam = &GameObject().AddComponent<CameraComponent>(float2(16, 9), 90.f);
	GameObject().AddComponent<EntityPickingComponent>();

	Expose(myCameraSensitivity, "Sensitivity", 0.01f);
}

void EditorController::Move()
{
	if (myMovement.LengthSqr() == 0)
		return;

	myMovement.Normalize();
	myMovement *= (mySpeed + (mySpeed * myShiftDown * (myShiftMult - 1.f))) * Singleton<Time>().deltaTime;
	GameObject().GetComponent<Transform>()->Move(myMovement, Transform::Space::Object);
	myMovement = { 0, 0, 0 };
}

void EditorController::Look()
{
	if (!myCanLook)
	{
		return;
	}

	float2 mDelta = Input::GetMouseDelta();

	myRotation += mDelta * myCameraSensitivity;

	myRotation.y = Math::Clamp(myRotation.y, -80.f, 80.f);

	GameObject().GetComponent<Transform>()->SetRotation({ myRotation.y, myRotation.x, 0 });

	myCanLook = false;
	Input::LockCursor(false);
}

void EditorController::SetCanLook()
{
	if (!GameObject().GetActive())
	{
		return;
	}

	myCanLook = true; 
	Input::LockCursor(true);
}

void EditorController::MoveInput(float3 aMovement)
{
	myMovement += aMovement;
}

void EditorController::MoveY(int aDirection)
{
	float4 yDir({ 0.f, (float)Math::Clamp(aDirection, -1, 1), 0.f, 0.f});
	float4x4 toWorldInverse = GameObject().GetComponent<Transform>()->GetTransform().FastInverse();
	float3 worldUpInObjectSpace = float4(yDir * toWorldInverse).xyz;
	myMovement += worldUpInObjectSpace;
}

void EditorController::Zoom(float aZoomValue)
{
	if (!MouseIsOverGameWindow())
		return;

	float newZoom = myCam->GetFOV() + (aZoomValue + (aZoomValue * myShiftDown * (myShiftMult - 1.f)));
	newZoom = Math::Clamp(newZoom, 15.f, 140.f);
	myCam->SetFOV(newZoom);
}

void EditorController::Pick()
{
	GameObject().GetComponent<EntityPickingComponent>()->Pick();
}

bool EditorController::MouseIsOverGameWindow()
{
	float2 pickPos = Input::GetMousePos();
	float4 gameRect = Singleton<GlobalSettings>().GetGameScreenNormalised();

	pickPos -= gameRect.xy;
	gameRect.zw -= gameRect.xy;
	gameRect.xy = { 0, 0 };

	pickPos.x /= gameRect.z;
	pickPos.y /= gameRect.w;
	pickPos.y = 1.f - pickPos.y;

	return (
		pickPos.x >= 0.f &&
		pickPos.x <= 1.f &&
		pickPos.y >= 0.f &&
		pickPos.y <= 1.f
		);
}
