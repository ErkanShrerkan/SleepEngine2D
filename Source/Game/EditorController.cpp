#include "pch.h"
#include "EditorController.h"
#include "Entity.h"
#include "CameraComponent.h"
#include "EntityPickingComponent.h"
#include "Transform.h"
#include "Globals.h"

EditorController::~EditorController()
{
}

void EditorController::Start()
{
	myIsObservingEditorInputs = true;
	eInputState ph = eInputState::Pressed | eInputState::Held;
	ObserveInputEvent(eInputEvent::Up, ph, [&]() { this->MoveInput({0, 1}); });
	ObserveInputEvent(eInputEvent::Down, ph, [&]() { this->MoveInput({0, -1}); });
	ObserveInputEvent(eInputEvent::Left, ph, [&]() { this->MoveInput({-1, 0}); });
	ObserveInputEvent(eInputEvent::Right, ph, [&]() { this->MoveInput({1, 0}); });
	ObserveInputEvent(eInputEvent::Shift, eInputState::Pressed, [&]() { this->ToggleShift(true); });
	ObserveInputEvent(eInputEvent::Shift, eInputState::Released, [&]() { this->ToggleShift(false); });
	ObserveInputEvent(eInputEvent::LMB, eInputState::Pressed, [&]() { this->Pick(); });
	ObserveScrollEvent(eScrollState::Up, [&]() { this->Zoom(-100); });
	ObserveScrollEvent(eScrollState::Down, [&]() { this->Zoom(100); });

	myCam = &GameObject().AddComponent<CameraComponent>(float2(16, 9), 1000.f);
	GameObject().AddComponent<EntityPickingComponent>();
}

void EditorController::Move()
{
	if (myMovement.LengthSqr() == 0)
		return;

	myMovement.Normalize();
	myMovement *= (mySpeed + (mySpeed * myShiftDown * (myShiftMult - 1.f))) * Singleton<Time>().deltaTime;
	GameObject().GetComponent<Transform>()->Move(myMovement, Transform::Space::Object);
	myMovement = { 0, 0 };
}

void EditorController::MoveInput(float2 aMovement)
{
	myMovement += aMovement;
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
