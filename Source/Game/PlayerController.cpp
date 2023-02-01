#include "pch.h"
#include "PlayerController.h"
#include "Entity.h"
#include "Sprite.h"
#include "Collider.h"
#include "Transform.h"

PlayerController::PlayerController(Entity*& myEntity)
	: Component::Component(myEntity)
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::Update(float aDeltaTime)
{
	aDeltaTime;
	//myMovement = { 0, 0 };
	//myMovement.y += Input::GetInputDown(eInputEvent::Down) - Input::GetInputDown(eInputEvent::Up);
	//myMovement.x += Input::GetInputDown(eInputEvent::Right) - Input::GetInputDown(eInputEvent::Left);
	myMovement.Normalize();
	myMovement *= mySpeed;
	myPosition += myMovement * Singleton<Time>().deltaTime;

	GameObject().GetComponent<Transform>().myTransform.SetRow(3, { myPosition, 1 });

	myMovement = { 0, 0 };
}

void PlayerController::Start()
{
	myPosition = { float(rand() % 10000) / 10000.f, float(rand() % 10000) / 10000.f };
	auto& c = GameObject().AddComponent<Sprite>("textures/sprites/tga/tgalogo_w.dds");
	c.SetSizeRelativeToImage({ .1f, .1f });
	c.SetPivot({ .5f, .5f });
	auto& cc = GameObject().AddComponent<Collider>(Collider::eType::Circle);
	cc.isRigidBody = true;
	cc.radius = c.GetSize().x;

	eInputState ph = eInputState::Pressed | eInputState::Held;
	ObserveInputEvent(eInputEvent::Up, ph, [&]() { this->MoveUp(); });
	ObserveInputEvent(eInputEvent::Down, ph, [&]() { this->MoveDown(); });
	ObserveInputEvent(eInputEvent::Left, ph, [&]() { this->MoveLeft(); });
	ObserveInputEvent(eInputEvent::Right, ph, [&]() { this->MoveRight(); });
	printe("PlayerController Started\n");
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
	myMovement.y -= 1;
}

void PlayerController::MoveDown()
{
	myMovement.y += 1;
}
