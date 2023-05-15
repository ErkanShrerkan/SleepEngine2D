#include "pch.h"
#include "Component.h"
#include "ComponentExposer.h"
#include "Entity.h"

Component::Component()
{
	myExposer = std::make_shared<ComponentExposer>();

}

Entity& Component::GameObject()
{
	return *myEntity;
}

void Component::SetActive(bool anActiveState)
{
	myIsActive = anActiveState;
}

bool Component::GetActive()
{
	return myIsActive;
}

void Component::ToggleActive()
{
	myIsActive = !myIsActive;
}

void Component::SetEntity(sptr(Entity) anEntity)
{
	myEntity = anEntity;
	myExposer->SetGameManager(&myEntity->GetGameManager());
}
