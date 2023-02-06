#include "pch.h"
#include "GameManager.h"
#include "Globals.h"
#include <Engine\LineDrawer.h>
#include "SystemInclude.h"
#include "ComponentInclude.h"
#include "Entity.h"
#include <Engine\DebugProfiler.h>

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
	// register components
	RegisterComponent<Sprite>("Sprite");
	RegisterComponent<Collider>("Collider");
	RegisterComponent<Transform>("Transform");
	RegisterComponent<PlayerController>("PlayerController");
	RegisterComponent<CameraComponent>("Camera");

	RegisterSystem<SpriteRenderSystem>();
	RegisterSystem<CollisionSystem>();

	for (size_t i = 0; i < 25; i++)
	{
		auto& e = CreateEntity();
		auto& s = e.AddComponent<Sprite>("textures/sprites/circle.dds");
		s.SetColor({ Random::Float(0.f, 1.f), Random::Float(0.f, 1.f), Random::Float(0.f, 1.f), 1 });
		s.SetSizeRelativeToImage({ .1f, .1f });
		e.GetComponent<Transform>().SetPosition({ Random::Float(-10.f, 10.f), Random::Float(-10.f, 10.f) });
	}

	auto& e = CreateEntity(); 
	e.AddComponent<PlayerController>();
	e.AddComponent<CameraComponent>();
	e.GetComponent<Transform>().SetPosition({ Random::Float(-10.f, 10.f), Random::Float(-10.f, 10.f) });

	printe("GameManager Inited\n");
}

void GameManager::Update()
{
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

void GameManager::OnImGui()
{
	// TODO: GameObjects selected with combo, then components and variables with table
	//ImGui::SetNextWindowSize(ImVec2(600, 450), ImGuiCond_FirstUseEver);
	ImGui::Separator();
	ImGui::Text("Inspector");
	//if (!ImGui::Begin())
	//{
	//	ImGui::End();
	//	return;
	//}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
	if (ImGui::BeginTable("", 3, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::Text("GameObjects");
		//ImGui::Separator();
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("Components");
		//ImGui::Separator();
		ImGui::TableSetColumnIndex(2);
		ImGui::Text("Variables");
		//ImGui::Separator();

		for (auto& [entity, components] : myEntityComponents)
		{
			ImGui::PushID(entity);
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			if (ImGui::TreeNode("Entities", "%s_%u", "Entity", entity))
			{
				// loop children when implemented
				//

				// loop components
				for (auto& [id, component] : components)
				{
					//if (component->HasExposedVariables())
					{
						ImGui::PushID(id);
						component->OnImGui(myComponentMaps[id]->myName);
						ImGui::PopID();
					}
				}
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleVar();

	Singleton<SE::Debug::CDebugProfiler>().Render();
	//ImGui::End();
}

Entity& GameManager::CreateEntity()
{
	uint id = myNextEntityID++;
	myEntities[id] = new Entity(id, this);
	Entity& entity = *myEntities[id];
	entity.AddComponent<Transform>();
	return entity;
}
