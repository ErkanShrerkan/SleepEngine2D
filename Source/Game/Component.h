#pragma once
#include <Engine\Input.h>
#include "ExposedComponent.h"

class Entity;
class Component : public ExposedComponent
{
	friend class GameManager;
public:
	Component(){}
	Entity& GameObject();
	virtual void Update(){};
	virtual void Start() = 0;
	virtual void Reload(){};

private:
	void SetEntity(Entity* anEntity)
	{
		myEntity = anEntity;
	}

private:
	Entity* myEntity = nullptr;
};

