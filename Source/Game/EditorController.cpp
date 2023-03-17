#include "pch.h"
#include "EditorController.h"
#include "Entity.h"
#include "CameraComponent.h"
#include "EntityPickingComponent.h"
#include "Transform.h"

EditorController::~EditorController()
{
	eInputState ph = eInputState::Pressed | eInputState::Held;
	StopObservingInputEvent(eInputEvent::Up, ph);
	StopObservingInputEvent(eInputEvent::Down, ph);
	StopObservingInputEvent(eInputEvent::Left, ph);
	StopObservingInputEvent(eInputEvent::Right, ph);
}

void EditorController::Start()
{
	myIsObservingEditorInputs = true;
	eInputState ph = eInputState::Pressed | eInputState::Held;
	ObserveInputEvent(eInputEvent::Up, ph, [&]() { this->MoveUp(); });
	ObserveInputEvent(eInputEvent::Down, ph, [&]() { this->MoveDown(); });
	ObserveInputEvent(eInputEvent::Left, ph, [&]() { this->MoveLeft(); });
	ObserveInputEvent(eInputEvent::Right, ph, [&]() { this->MoveRight(); });

	GameObject().AddComponent<CameraComponent>(float2(16, 9), 1000.f);
	GameObject().AddComponent<EntityPickingComponent>();
}

void EditorController::Move()
{
	if (myMovement.LengthSqr() > 0)
	{
		myMovement.Normalize();
		myMovement *= mySpeed * Singleton<Time>().deltaTime;
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
