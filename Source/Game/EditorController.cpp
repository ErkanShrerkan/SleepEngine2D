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
	ObserveInputEvent(eInputEvent::Up, ph, [&]() { this->MoveUp(); });
	ObserveInputEvent(eInputEvent::Down, ph, [&]() { this->MoveDown(); });
	ObserveInputEvent(eInputEvent::Left, ph, [&]() { this->MoveLeft(); });
	ObserveInputEvent(eInputEvent::Right, ph, [&]() { this->MoveRight(); });
	ObserveInputEvent(eInputEvent::Shift, eInputState::Pressed, [&]() { this->ToggleShift(true); });
	ObserveInputEvent(eInputEvent::Shift, eInputState::Released, [&]() { this->ToggleShift(false); });
	ObserveInputEvent(eInputEvent::LMB, eInputState::Pressed, [&]() { this->Pick(); });
	ObserveScrollEvent(eScrollState::Up, [&]() {this->ZoomIn(); });
	ObserveScrollEvent(eScrollState::Down, [&]() {this->ZoomOut(); });

	myCam = &GameObject().AddComponent<CameraComponent>(float2(16, 9), 1000.f);
	GameObject().AddComponent<EntityPickingComponent>();
}

void EditorController::Move()
{
	if (myMovement.LengthSqr() > 0)
	{
		myMovement.Normalize();
		myMovement *= mySpeed * Singleton<Time>().deltaTime * (myShiftDown + 1);
		GameObject().GetComponent<Transform>()->Move(myMovement, Transform::Space::Object);
		myMovement = { 0, 0 };
	}
}

void EditorController::MoveRight()
{
	myMovement.x += 1;
}

void EditorController::MoveLeft()
{
	myMovement.x -= 1;
}

void EditorController::MoveUp()
{
	myMovement.y += 1;
}

void EditorController::MoveDown()
{
	myMovement.y -= 1;
}

void EditorController::ZoomOut()
{
	if (!MouseIsOverGameWindow())
		return;

	float newZoom = myCam->GetZoom() + (100 * (myShiftDown + 1));
	myCam->SetZoom(newZoom);
}

void EditorController::ZoomIn()
{
	if (!MouseIsOverGameWindow())
		return;

	float newZoom = myCam->GetZoom() - (100 * (myShiftDown + 1));
	newZoom = newZoom < 0 ? 0 : newZoom;
	myCam->SetZoom(newZoom);
}

void EditorController::Pick()
{
	GameObject().GetComponent<EntityPickingComponent>()->Pick();
}

bool EditorController::MouseIsOverGameWindow()
{
	float2 pickPos = Input::GetMousePos();
	float4 gameRect = Singleton<GlobalSettings>().gameWindowRect;

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
