#pragma once
#include <Engine\Input.h>
#include "ComponentExposer.h"
#include "IComponent.h"

#define OVERRIDED(aBase, aDerived, aFunction) !std::is_same_v<decltype(&aBase::aFunction), decltype(&aDerived::aFunction)>

class Entity;
class Component : public IComponent
{
	friend class GameManager;
public:
	Component();
	~Component(){ /*printf("\nCOMPONENT DELETED\n");*/ }
	Entity& GameObject();
	ComponentExposer* GetComponentExposer() { return myExposer.get(); }
	virtual void Update(){};
	virtual void Start() = 0;
	virtual void Reload(){};

protected:
	template <typename... Args>
	void Expose(Args&&... someArgs)
	{
		myExposer->Expose(std::forward<Args>(someArgs)...);
	}

private:
	void SetEntity(Entity* anEntity);

private:
	Entity* myEntity = nullptr;
	sptr(ComponentExposer) myExposer;
};
