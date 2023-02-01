#pragma once
#include <Engine\Input.h>

class Entity;

class Component
{
	//friend class InputObserver;
public:
	Component(Entity*& anEntity);
	Entity& GameObject();
	virtual void Update([[maybe_unused]] float aDeltaTime){};
	virtual void Start() = 0;

protected:
	Entity*& myEntity;
};

