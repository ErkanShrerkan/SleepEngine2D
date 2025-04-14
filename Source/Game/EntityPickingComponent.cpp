#include "pch.h"
#include "EntityPickingComponent.h"

EntityPickingComponent::~EntityPickingComponent()
{
}

void EntityPickingComponent::Start()
{
	eInputState ph = eInputState::Released;
	myInputObserver.ObserveInputEvent(eInputEvent::LMB, ph, [&]() { Pick(); });
}

void EntityPickingComponent::Pick()
{
	myShouldPick = true;
}
