#pragma once
#include "GameManager.h"

class Entity
{
public:
	Entity(){}
	Entity(uint anID, GameManager* aGM)
		: myID(anID), myGameManager(aGM) {}

	template <typename ComponentType, typename... Args>
	typename std::enable_if<std::is_base_of<Component, ComponentType>::value, ComponentType&>::type
	AddComponent(Args&&... someArgs)
	{
		return myGameManager->AddComponent<ComponentType>(myID, std::forward<Args>(someArgs)...);
	}

	template <typename ComponentType>
	typename std::enable_if<std::is_base_of<Component, ComponentType>::value, ComponentType&>::type
	GetComponent()
	{
		return myGameManager->GetComponent<ComponentType>(myID);
	}

private:
	uint myID;
	GameManager* myGameManager;
};