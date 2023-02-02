#include "pch.h"
#include "GameManager.h"
#include "Globals.h"
#include <Engine\LineDrawer.h>
#include "SystemInclude.h"
#include "ComponentInclude.h"
#include "Entity.h"

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

	printe("GameManager Deleted\n");
}

void GameManager::Init()
{
	myComponentMaps[GetID<Sprite>()] = new ComponentMap<Sprite>();
	myComponentMaps[GetID<Collider>()] = new ComponentMap<Collider>();
	myComponentMaps[GetID<Transform>()] = new ComponentMap<Transform>();
	myComponentMaps[GetID<PlayerController>()] = new ComponentMap<PlayerController>();
	//myComponentMaps[GetID<CameraComponent>()] = new ComponentMap<CameraComponent>();
	RegisterSystem<SpriteRenderSystem>();
	RegisterSystem<CollisionSystem>();

	for (size_t i = 0; i < 100; i++)
	{
		CreateEntity().AddComponent<PlayerController>();
	}

	printe("GameManager Inited\n");
}

void GameManager::Update(float aDeltaTime)
{
	Singleton<Time>().deltaTime = aDeltaTime;
	//printf("%f\n", 1.f / aDeltaTime);

	for (auto& system : mySystems)
	{
		system->Update();
	}

	for (auto& [componentID, componentMap] : myComponentMaps)
	{
		componentMap->UpdateComponents();
	}

	uint2 res = Singleton<GlobalSettings>().gameplayResolution;
	for (uint x = 0; x < res.x; x++)
	{
		Debug::DrawLine2D({ (float)x / res.x, 0 }, { (float)x / res.x, 1 }, { 1, 1, 1, .0125f });
	}
	for (uint y = 0; y < res.y; y++)
	{
		Debug::DrawLine2D({ 0, (float)y / res.y }, { 1, (float)y / res.y }, { 1, 1, 1, .0125f });
	}
}

Entity& GameManager::CreateEntity()
{
	uint id = myNextEntityID++;
	myEntities[id] = new Entity(id, this);
	Entity& entity = *myEntities[id];
	entity.AddComponent<Transform>();
	return entity;
}
