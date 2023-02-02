#pragma once
#include <Engine\Input.h>

class Entity;

class Component
{
	//friend class InputObserver;
public:
	Component(Entity*& anEntity);
	Entity& GameObject();
	virtual void Update(){};
	virtual void Start() = 0;

protected:
	Entity*& myEntity;
};

