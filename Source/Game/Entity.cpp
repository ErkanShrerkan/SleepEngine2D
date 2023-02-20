#include "pch.h"
#include "Entity.h"

Entity& Entity::GetParent()
{
	return GetEntity(myParentID);
}

Entity& Entity::AdoptChild(Entity& anEntity)
{
	// Last parent abandons child
	ForceAbandon(anEntity.myParentID, anEntity.myID);
	// Child is free to adopt
	myChildren.push_back(anEntity.myID);
	anEntity.myParentID = myID;
	return anEntity;
}

Entity& Entity::AdoptChild(uint anID)
{
	// Last parent abandons child
	Entity& child = GetEntity(anID);
	ForceAbandon(child.myParentID, anID);
	// Child is free to adopt
	myChildren.push_back(anID);
	child.myParentID = myID;
	return child;
}

Entity& Entity::CreateChild()
{
	return AdoptChild(CreateEntity());
}

Entity& Entity::AbandonChild(uint anID)
{
	myChildren.erase(
		std::remove_if(myChildren.begin(), myChildren.end(),
			[&](uint& aChildID)
			{
				return aChildID == anID;
			}),
		myChildren.end());
	Entity& e = GetEntity(anID);
	e.myParentID = UINT_MAX;
	return e;
}

Entity& Entity::GetChild(uint anID)
{
	for (auto& id : myChildren)
	{
		if (anID == id)
		{
			return GetEntity(anID);
		}
	}

	return *this;
}

void Entity::RemoveChild(uint anID)
{
	Entity& child = GetChild(anID);
	if (child.GetID() == myID)
		return;

	child.MarkForRemoval();
}

uint Entity::GetID()
{
	return myID;
}

uint Entity::GetParentID()
{
	return myParentID;
}

std::vector<uint> Entity::GetChildrenIDs()
{
	return myChildren;
}

Entity& Entity::GetEntity(uint anID)
{
	return myGameManager->GetEntity(anID);
}

Entity& Entity::CreateEntity()
{
	return myGameManager->CreateEntity();
}

void Entity::MarkForRemoval()
{
	if (myParentID != UINT_MAX)
	{
		GetParent().AbandonChild(myID);
	}

	myGameManager->MarkEntityForRemoval(myID);
	for (auto& id : myChildren)
	{
		GetEntity(id).MarkForRemoval();
	}
}

void Entity::ForceAbandon(uint aParentID, uint aChildID)
{
	if (aParentID == UINT_MAX)
		return;

	GetEntity(aParentID).AbandonChild(aChildID);
}
