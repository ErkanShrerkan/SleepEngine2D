#include "pch.h"
#include "Component.h"
#include "Entity.h"

Component::Component(Entity*& anEntity)
	: myEntity(anEntity)
{
}

Entity& Component::GameObject()
{
	return *myEntity;
}
