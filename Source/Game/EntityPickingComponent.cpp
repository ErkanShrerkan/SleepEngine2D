#include "pch.h"
#include "EntityPickingComponent.h"

EntityPickingComponent::~EntityPickingComponent()
{
}

void EntityPickingComponent::Start()
{
	eInputState ph = eInputState::Released;
	ObserveInputEvent(eInputEvent::LMB, ph, [&]() { this->Pick(); });
}

void EntityPickingComponent::Pick()
{
	myShouldPick = true;
}
