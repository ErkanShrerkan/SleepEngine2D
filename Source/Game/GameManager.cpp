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

GameManager::GameManager()
{
	//myShowChildrenRecord[UINT_MAX] = true;
	mySceneManager = new SceneManager(this);
}

GameManager::~GameManager()
{
	for (auto& [id, entity] : myEntities)
	{
		delete entity;
		for (auto& [type, pointer] : myEntityComponents[id])
		{
			myComponentMaps[type]->DeleteComponentFromEntity(id);
		}
	}

	for (auto& [id, mcm] : myComponentMaps)
	{
		delete mcm;
	}

	delete mySceneManager;

	printe("GameManager Deleted\n");
}

void GameManager::Init()
{
	// TODO: Implement Pixel Art Animation Texture Map Lookup Thing

	// register components
	RegisterComponent<EditorController>("Editor Controller", true);
	RegisterComponent<EntityPickingComponent>("Entity Picking", true);
	RegisterComponent<Sprite>("Sprite");
	RegisterComponent<Collider>("Collider");
	RegisterComponent<Transform>("Transform");
	RegisterComponent<PlayerController>("Player Controller");
	RegisterComponent<CameraComponent>("Camera");
	RegisterComponent<Rigidbody>("Rigidbody");

	RegisterSystem<EditorSystem>();
	RegisterSystem<SpriteRenderSystem>();
	RegisterSystem<CollisionSystem>();
	RegisterSystem<CameraSystem>();
	RegisterSystem<EntityPickingSystem>();

	auto& entityPool = CreateEntity();
	for (size_t i = 0; i < 100; i++)
	{
		auto& e = entityPool.CreateChild();
		e.GetComponent<Transform>()->SetPosition({ Random::Float(-10'00.f, 10'00.f), Random::Float(-10'00.f, 10'00.f) });
		//e.GetComponent<Transform>().SetScale({ .1f, .1f });
		auto& s = e.AddComponent<Sprite>("assets/textures/sprites/circle.dds");
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
		SetColor({0, 1, 0, 1});

	printe("GameManager Inited\n");
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

	int2 res = { 100, 100 };
	for (int x = -res.x; x < res.x; x++)
	{
		Debug::DrawLine2D({ x * 100.f, -res.x * 100.f }, { x * 100.f, res.x * 100.f }, { 1, 1, 1, .0125f }, true);
	}
	for (int y = -res.y; y < res.y; y++)
	{
		Debug::DrawLine2D({ -res.y * 100.f, y * 100.f }, { res.y * 100.f, y * 100.f }, { 1, 1, 1, .0125f }, true);
	}
}

Entity& GameManager::CreateEntity()
{
	uint id = myNextEntityID++;
	myEntities[id] = new Entity(id, this);
	Entity& entity = *myEntities[id];
	entity.AddComponent<Transform>();
	UpdateHierarchy();
	return entity;
}

void GameManager::RemoveEntity(uint anEntityID)
{
	// deletes the entity
	delete myEntities[anEntityID];
	// removes the entity entry
	myEntities.erase(anEntityID);

	// delete and remove the entity's components
	for (auto& [componentID, component] : myEntityComponents[anEntityID])
	{
		myComponentMaps[componentID]->DeleteComponentFromEntity(anEntityID);
	}

	myEntityComponents.erase(anEntityID);

	UpdateHierarchy();
}

void GameManager::MarkEntityForRemoval(uint anEntityID)
{
	Entity& e = GetEntity(anEntityID);
	if (e.GetParentID() != UINT_MAX)
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
		entity->MarkForRemoval();
	}
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
		componentMap->UpdateComponents();
	}
}

void GameManager::UpdateHierarchy(bool aBool)
{
	myEntityHierarchyNeedsUpdating = aBool;
}

