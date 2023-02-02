#include "pch.h"
#include "Collider.h"

Collider::Collider(eType aType, Entity*& myEntity)
	: Component::Component(myEntity)
{
	aType;
}

Collider::~Collider()
{
}

void Collider::Update()
{
	// set position to position relative to parent
}

void Collider::Start()
{
}
