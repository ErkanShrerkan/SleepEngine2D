#pragma once
#include <Engine\Input.h>
#include "ExposedComponent.h"

#define OVERRIDED(aBase, aDerived, aFunction) !std::is_same_v<decltype(&aBase::aFunction), decltype(&aDerived::aFunction)>

class Entity;
class Component : public ExposedComponent
{
	friend class GameManager;
public:
	Component(){}
	~Component(){ /*printf("\nCOMPONENT DELETED\n");*/ }
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

