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
	typename std::enable_if<std::is_base_of<Component, ComponentType>::value, ComponentType*>::type
	GetComponent()
	{
		return myGameManager->GetComponent<ComponentType>(myID);
	}

	Entity& GetParent();
	Entity& AdoptChild(Entity& anEntity);
	Entity& AdoptChild(uint anID);
	Entity& CreateChild();
	Entity& AbandonChild(uint anID);
	Entity& GetChild(uint anID);
	Entity& GetEntity(uint anID);
	Entity& CreateEntity();
	GameManager& GetGameManager() { return *myGameManager; }
	std::vector<uint> GetChildrenIDs();
	uint GetID();
	uint GetParentID();
	void MarkForRemoval();
	void RemoveChild(uint anID);
	bool HasParent();

private:
	void ForceAbandon(uint aParentID, uint aChildID);

private:
	uint myID;
	uint myParentID = NULL_ENTITY;
	GameManager* myGameManager;
	std::vector<uint> myChildren;
};