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

	//RegisterSystem<SpriteRenderSystem>();
	//RegisterSystem<CollisionSystem>();

	for (size_t i = 0; i < 10; i++)
	{
		auto& e = CreateEntity();
		e.GetComponent<Transform>().SetPosition({ Random::Float(-10.f, 10.f), Random::Float(-10.f, 10.f) });
		e.GetComponent<Transform>().SetScale({ .1f, .1f });
		auto& s = e.AddComponent<Sprite>("textures/sprites/circle.dds");
		s.SetColor({ Random::Float(0.f, 1.f), Random::Float(0.f, 1.f), Random::Float(0.f, 1.f), 1 });
		//s.SetSizeRelativeToImage({ .1f, .1f });
	}

	CreateEntity().AddComponent<PlayerController>();
	printe("GameManager Inited\n");
}

void GameManager::Update()
{
	for (auto& entityID : myEntitiesToRemove)
	{
		RemoveEntity(entityID);
	}
	myEntitiesToRemove.clear();

	for (auto& system : mySystems)
	{
		system->Update();
	}

	for (auto& [componentID, componentMap] : myComponentMaps)
	{
		componentMap->UpdateComponents();
	}

	//uint2 res = Singleton<GlobalSettings>().gameplayResolution;
	//for (uint x = 0; x < res.x; x++)
	//{
	//	Debug::DrawLine2D({ (float)x / res.x, 0 }, { (float)x / res.x, 1 }, { 1, 1, 1, .0125f });
	//}
	//for (uint y = 0; y < res.y; y++)
	//{
	//	Debug::DrawLine2D({ 0, (float)y / res.y }, { 1, (float)y / res.y }, { 1, 1, 1, .0125f });
	//}
}

void GameManager::OnImGui()
{
	ImGui::Separator();
	ImGui::Text("Inspector");
	ImGui::PushStyleColor(ImGuiCol_Button, { .1f, .1f, .1f, 1 });
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
	if (ImGui::BeginTable("", 2, ImGuiTableFlags_BordersOuter))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Game Objects");
		ImGui::SameLine();
		AddEntity();
		SelectEntity();
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("");
		AddEntityComponent();
		ImGui::EndTable();
	}
	ModifyValues();
	Singleton<SE::Debug::CDebugProfiler>().Render();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

Entity& GameManager::CreateEntity()
{
	uint id = myNextEntityID++;
	myEntities[id] = new Entity(id, this);
	Entity& entity = *myEntities[id];
	entity.AddComponent<Transform>();
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

	if (mySelectedEntity == anEntityID)
	{
		mySelectedEntity = UINT_MAX;
	}
}

void GameManager::MarkEntityForRemoval(uint anEntityID)
{
	myEntitiesToRemove.insert(anEntityID);
}

void GameManager::AddEntityComponent()
{
	if (!ValidSelection())
	{
		return;
	}

	if (ImGui::Button("Reload"))
	{
		if (ValidSelection())
		{
			for (auto& [id, component] : myEntityComponents[mySelectedEntity])
			{
				component->Reload();
			}
		}
	};
	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		if (ValidSelection())
		{
			MarkEntityForRemoval(mySelectedEntity);
		}
	};

	//ImGui::SameLine();
	std::set<uint> entityComponents;
	for (auto& [componentID, component] : myEntityComponents[mySelectedEntity])
	{
		entityComponents.insert(componentID);
	}

	if (ImGui::BeginCombo("", "Add Component"))
	{
		for (auto& [componentID, map] : myComponentMaps)
		{
			// can't add a component that already has been added
			if (entityComponents.find(componentID) != entityComponents.end())
			{
				continue;
			}

			if (ImGui::MenuItem(map->GetName().c_str()))
			{
				map->AddComponent(mySelectedEntity, *this);
			}
		}
		ImGui::EndMenu();
	}
}

void GameManager::AddEntity()
{
	if (ImGui::Button("Add Object"))
	{
		CreateEntity();
	}
}

void GameManager::SelectEntity()
{
	if (ImGui::BeginListBox("", { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 3 }))
	{
		for (auto& [entity, components] : myEntityComponents)
		{
			ImGuiTreeNodeFlags nodeFlag = ImGuiTreeNodeFlags_Leaf;
			if (mySelectedEntity == entity)
			{
				nodeFlag |= ImGuiTreeNodeFlags_Selected;
			}

			if (ImGui::TreeNodeEx("Entities", nodeFlag, "%s_%u", "Entity", entity))
			{
				ImGui::PushID(entity);
				if (ImGui::IsItemClicked())
				{
					if (mySelectedEntity == entity)
					{
						mySelectedEntity = UINT_MAX;
					}
					else
					{
						mySelectedEntity = entity;
					}
				}
				ImGui::PopID();
				ImGui::TreePop();
			}
		}
		ImGui::EndListBox();
	}
}

void GameManager::ModifyValues()
{
	if (!ValidSelection())
	{
		return;
	}

	if (ImGui::BeginTable("", 2, ImGuiTableFlags_BordersOuter))
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Components");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("Variables");
		// loop children when implemented
		//

		// loop components
		auto& components = myEntityComponents[mySelectedEntity];
		for (auto& [id, component] : components)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			//if (component->HasExposedVariables())
			{
				ImGui::PushID(id);
				component->OnImGui(myComponentMaps[id]->myName);
				ImGui::PopID();
			}
		}
		ImGui::EndTable();
	}
}

bool GameManager::ValidSelection()
{
	return mySelectedEntity != UINT_MAX;
}
