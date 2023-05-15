#include "pch.h"
#include "GameManager.h"
#include "Globals.h"
#include <Engine\LineDrawer.h>
#include "Entity.h"
#include <Engine\JsonDocument.h>
#include <Engine\DebugProfiler.h>
#include <fstream>
#include "SystemInclude.h"
#include "ComponentInclude.h"
#include "SceneManager.h"
#include "ComponentExposer.h"

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
	//for (auto& [id, entity] : myEntities)
	//{
	//	delete entity;
	//}
	//printe("GameManager Deleted\n");
}

void GameManager::Init()
{
	// TODO: Implement Pixel Art Animation Texture Map Lookup Thing

	// include files to make it easier to modify the files
	#include "ComponentRegister.h"
	#include "SystemRegister.h"

	mySceneManager = std::make_shared<SceneManager>(this);
	mySceneManager->LoadScene();

	auto& entityPool = CreateEntity();
	for (size_t i = 0; i < 100; i++)
	{
		auto& e = entityPool.CreateChild();
		e.GetComponent<Transform>()->SetPosition({ Random::Float(-10'00.f, 10'00.f), Random::Float(-10'00.f, 10'00.f) });
		//e.GetComponent<Transform>().SetScale({ .1f, .1f });
		auto& s = e.AddComponent<Sprite>(/*"assets/textures/black.dds"*/"assets/textures/sprites/circle.dds");
		s.SetColor({ Random::Float(0.f, 1.f), Random::Float(0.f, 1.f), Random::Float(0.f, 1.f), 1 });
		s.SetWidthSizePreservedImageRatio(100);
		s.SetPivot({ .5f, .5f });
		//s.SetSizeRelativeToImage({ .1f, .1f });

		for (size_t j = 0; j < 100; j++)
		{
			e.CreateChild();
		}
	}

	CreateEntity().AddComponent<PlayerController>().GameObject().
		CreateChild().
		CreateChild().
		CreateChild().
		CreateChild().
		CreateChild().
		CreateChild().
		CreateChild().
		AddComponent<Sprite>("assets/textures/sprites/circle.dds").
		SetWidthSizePreservedImageRatio(125).
		SetColor({ 0, 1, 0, 1 });

	printe("GameManager [INITED]\n");
}

void GameManager::Update()
{
	UpdateEntityRemoval();
	UpdateSystems();
	UpdateComponents();

	//uint2 res = Singleton<GlobalSettings>().gameplayResolution;
	//for (uint x = 0; x < res.x; x++)
	//{
	//	Debug::DrawLine2D({ (float)x / res.x, 0 }, { (float)x / res.x, 1 }, { 1, 1, 1, .0125f });
	//}
	//for (uint y = 0; y < res.y; y++)
	//{
	//	Debug::DrawLine2D({ 0, (float)y / res.y }, { 1, (float)y / res.y }, { 1, 1, 1, .0125f });
	//}

	//Debug::DrawLine2D({ 0, 0 }, { 100, 100 }, { 1, 1, 1, 1 });
}

void GameManager::SaveScene()
{
	mySceneManager->SaveScene();
}

void GameManager::SaveSceneAs(const std::string& aSceneName)
{
	mySceneManager->SaveSceneAs(aSceneName);
}

void GameManager::LoadScene(const std::string& aSceneName)
{
	mySceneManager->LoadScene(aSceneName);
}

void GameManager::RemoveRefsToComponent(Component* aComponent)
{
	auto it = myRegisteredComponentRefs.find(aComponent);
	if (it == myRegisteredComponentRefs.end())
		return;

	for (void** ref : it->second)
	{
		*ref = nullptr;
	}

	myRegisteredComponentRefs.erase(aComponent);
}

void GameManager::RegisterComponentRef(void** aRefAddress, Component* aReferencedComponent)
{
	myRegisteredComponentRefs[aReferencedComponent].insert(aRefAddress);
}

Entity& GameManager::CreateEntity()
{
	uint id = myNextEntityID++;
	myEntities[id] = std::make_shared<Entity>(id, this);
	Entity& entity = *myEntities[id];
	entity.AddComponent<Transform>();
	UpdateHierarchy();
	return entity;
}

void GameManager::RemoveEntity(uint anEntityID)
{
	// removes the entity
	myEntities.erase(anEntityID);

	// delete and remove the entity's components
	for (auto& [componentID, component] : myEntityComponents[anEntityID])
	{
		RemoveRefsToComponent(component);
		myComponentMaps[componentID]->DeleteComponentFromEntity(anEntityID);
	}
	myEntityComponents.erase(anEntityID);

	UpdateHierarchy();
}

void GameManager::MarkEntityForRemoval(uint anEntityID)
{
	Entity& e = GetEntity(anEntityID);
	if (e.HasParent())
	{
		e.GetParent().AbandonChild(anEntityID);
	}
	myEntitiesToRemove.insert(anEntityID);
	for (auto& id : e.GetChildrenIDs())
	{
		MarkEntityForRemoval(id);
	}
}

void GameManager::UnLoadAll()
{
	for (auto& [id, entity] : myEntities)
	{
		MarkEntityForRemoval(id);
	}
	UpdateEntityRemoval();
}

void GameManager::InitComponent(Component& aComponent)
{
	aComponent.myExposer->Expose(aComponent.myIsActive, "Active");
}

bool GameManager::IsEntityActive(uint anEntityID)
{
	return GetEntity(anEntityID).GetActive();
}

bool GameManager::IsComponentActive(Component& aComponent)
{
	return aComponent.GetActive();
}

bool GameManager::IsEntityAndComponentActive(uint anEntityID, Component& aComponent)
{
	return !(!IsComponentActive(aComponent) || !IsEntityActive(anEntityID));
}

void GameManager::UpdateEntityRemoval()
{
	for (auto& entityID : myEntitiesToRemove)
	{
		RemoveEntity(entityID);
	}
	myEntitiesToRemove.clear();
}

void GameManager::UpdateSystems()
{
	for (auto& system : mySystems)
	{
		system->Update();
	}
}

void GameManager::UpdateComponents()
{
	for (auto& [componentID, componentMap] : myComponentMaps)
	{
		componentMap->UpdateComponents(*this);
	}
}

void GameManager::UpdateHierarchy(bool aBool)
{
	myEntityHierarchyNeedsUpdating = aBool;
}

