#pragma once
#include <Engine\Input.h>
#include "ExposedComponent.h"

class Entity;
class Component : public ExposedComponent
{
public:
	Component(Entity*& anEntity);
	Entity& GameObject();
	virtual void Update(){};
	virtual void Start() = 0;

private:
	Entity*& myEntity;
};

