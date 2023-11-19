#include "pch.h"
#include "EditorController.h"
#include "Entity.h"
#include "CameraComponent.h"
#include "EntityPickingComponent.h"
#include "Transform.h"
#include "Globals.h"

EditorController::~EditorController()
{
	StopObservingAllEvents();
}

void EditorController::Start()
{
	myIsObservingEditorInputs = true;
	eInputState ph = eInputState::Pressed | eInputState::Held;

	// movement
	ObserveInputEvent(eInputEvent::Forward, ph, [&]() { this->MoveInput({0, 0, 1}); });
	ObserveInputEvent(eInputEvent::Back, ph, [&]() { this->MoveInput({0, 0, -1}); });
	ObserveInputEvent(eInputEvent::Left, ph, [&]() { this->MoveInput({-1, 0, 0}); });
	ObserveInputEvent(eInputEvent::Right, ph, [&]() { this->MoveInput({1, 0, 0}); });
	ObserveInputEvent(eInputEvent::E, ph, [&]() { this->MoveInput({ 0, 1, 0 }); });
	ObserveInputEvent(eInputEvent::Q, ph, [&]() { this->MoveInput({ 0, -1, 0 }); });
	ObserveInputEvent(eInputEvent::Space, ph, [&]() { this->MoveY(1); });
	ObserveInputEvent(eInputEvent::Control, ph, [&]() { this->MoveY(-1); });
	ObserveInputEvent(eInputEvent::Shift, eInputState::Pressed, [&]() { this->ToggleShift(true); });
	ObserveInputEvent(eInputEvent::Shift, eInputState::Released, [&]() { this->ToggleShift(false); });

	// camera
	ObserveInputEvent(eInputEvent::RMB, ph, [&]() { this->SetCanLook(); });
	ObserveScrollEvent(eScrollState::Up, [&]() { this->Zoom(-100); });
	ObserveScrollEvent(eScrollState::Down, [&]() { this->Zoom(100); });

	ObserveInputEvent(eInputEvent::LMB, eInputState::Pressed, [&]() { this->Pick(); });

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

	float newZoom = myCam->GetZoom() + (aZoomValue + (aZoomValue * myShiftDown * (myShiftMult - 1.f)));
	myCam->SetZoom(newZoom < 100 ? 100 : newZoom);
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
