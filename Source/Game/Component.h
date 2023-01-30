#pragma once
class Entity;

class Component
{
public:
	Component(Entity*& anEntity);
	Entity& GameObject();
	virtual void Update([[maybe_unused]] float aDeltaTime){};
	virtual void Start() = 0;

protected:
	Entity*& myEntity;
};

