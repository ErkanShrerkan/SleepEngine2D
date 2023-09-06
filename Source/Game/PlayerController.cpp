#include "pch.h"
#include "PlayerController.h"
#include "Entity.h"
#include "Sprite.h"
#include "Collider.h"
#include "Transform.h"
#include "CameraComponent.h"
#include <Engine\LineDrawer.h>

PlayerController::PlayerController()
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::Update()
{
	Debug::DrawCircle(GameObject().GetComponent<Transform>()->GetPosition(), 50);

	if (myMovement.LengthSqr() == 0)
	{
		return;
	}

	myMovement.Normalize();
	myMovement *= mySpeed * Singleton<Time>().deltaTime;
	GameObject().GetComponent<Transform>()->Move(myMovement, Transform::Space::Object);
	myMovement = { 0, 0 };
}

void PlayerController::Start()
{
	//GameObject().GetComponent<Transform>().SetPosition({ 0, 0 }/*{ float(rand() % 10000) / 10000.f, float(rand() % 10000) / 10000.f }*/);
	auto& c = GameObject().AddComponent<Sprite>(/*"textures/sprites/white.dds"*//*"textures/sprites/tga/tgalogo_w.dds"*/);
	c.SetTexture("assets/textures/sprites/objects/pickup.dds");
	c.Reload();
	c.SetPivot({ .5f, .5f });
	c.SetSizeRelativeToImage({ .1f, .1f });
	c.SetWidthSizePreservedImageRatio(100); // 1m
	//auto& cc = GameObject().AddComponent<Collider>(Collider::eType::Circle);
	//cc.isRigidBody = true;
	//cc.radius = c.GetSize().x;
	GameObject().AddComponent<CameraComponent>(float2(16, 9), 1000.f);
	//GameObject().GetComponent<Transform>().SetPosition({ Random::Float(-10.f, 10.f), Random::Float(-10.f, 10.f) });
	ObserveInputEvent(eInputEvent::Interact, eInputState::Released, [&]() { this->ToggleMovement(); });

	ExposeEnable();
	Expose(mySpeed, "Speed", 10.f);
	Expose(myTransformRef, "Transform Ref Test");
}

void PlayerController::MoveRight()
{
	myMovement.x += 1;
}

void PlayerController::MoveLeft()
{
	myMovement.x -= 1;
}

void PlayerController::MoveUp()
{
	myMovement.y += 1;
}

void PlayerController::MoveDown()
{
	myMovement.y -= 1;
}

void PlayerController::ToggleMovement()
{
	myCanMove = !myCanMove;
	eInputState ph = eInputState::Pressed | eInputState::Held;
	if (myCanMove)
	{
		ObserveInputEvent(eInputEvent::Up, ph, [&]() { this->MoveUp(); });
		ObserveInputEvent(eInputEvent::Down, ph, [&]() { this->MoveDown(); });
		ObserveInputEvent(eInputEvent::Left, ph, [&]() { this->MoveLeft(); });
		ObserveInputEvent(eInputEvent::Right, ph, [&]() { this->MoveRight(); });
	}
	else
	{
		StopObservingInputEvent(eInputEvent::Up, ph);
		StopObservingInputEvent(eInputEvent::Down, ph);
		StopObservingInputEvent(eInputEvent::Left, ph);
		StopObservingInputEvent(eInputEvent::Right, ph);
	}
}
