#include "pch.h"
#include "Component.h"
#include "Entity.h"

Entity& Component::GameObject()
{
	return *myEntity;
}
