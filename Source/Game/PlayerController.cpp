#include "pch.h"
#include "PlayerController.h"
#include "Entity.h"
#include <Engine\Input.h>
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
	myMovement = { 0, 0 };
	myMovement.y += Input::GetInputDown(eButtonInput::Down) - Input::GetInputDown(eButtonInput::Up);
	myMovement.x += Input::GetInputDown(eButtonInput::Right) - Input::GetInputDown(eButtonInput::Left);
	myMovement.Normalize();
	myMovement *= mySpeed;
	myPosition += myMovement * aDeltaTime;

	GameObject().GetComponent<Transform>().myTransform.SetRow(3, { myPosition, 1 });
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
	printe("PlayerController Started\n");
}
