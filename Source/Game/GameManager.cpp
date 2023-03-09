#include "pch.h"
#include "GameManager.h"
#include "Globals.h"
#include <Engine\LineDrawer.h>
#include "SystemInclude.h"
#include "ComponentInclude.h"
#include "Entity.h"
#include <Engine\JsonDocument.h>
#include <Engine\DebugProfiler.h>
#include <fstream>
#include "SceneManager.h"

GameManager::GameManager()
{
	myShowChildrenRecord[UINT_MAX] = true;
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
	// register components
	RegisterComponent<Sprite>("Sprite");
	RegisterComponent<Collider>("Collider");
	RegisterComponent<Transform>("Transform");
	RegisterComponent<PlayerController>("PlayerController");
	RegisterComponent<CameraComponent>("Camera");

	//RegisterSystem<SpriteRenderSystem>();
	//RegisterSystem<CollisionSystem>();

	auto& entityPool = CreateEntity();
	for (size_t i = 0; i < 100; i++)
	{
		auto& e = entityPool.CreateChild();
		e.GetComponent<Transform>().SetPosition({ Random::Float(-100'00.f, 100'00.f), Random::Float(-100'00.f, 100'00.f) });
		//e.GetComponent<Transform>().SetScale({ .1f, .1f });
		auto& s = e.AddComponent<Sprite>("textures/sprites/circle.dds");
		s.SetColor({ Random::Float(0.f, 1.f), Random::Float(0.f, 1.f), Random::Float(0.f, 1.f), 1 });
		s.SetWidthSizePreservedImageRatio(100);
		//s.SetSizeRelativeToImage({ .1f, .1f });

		for (size_t j = 0; j < 100; j++)
		{
			e.CreateChild();
		}
	}

	CreateEntity().AddComponent<PlayerController>().GameObject().CreateChild().CreateChild().CreateChild().CreateChild().CreateChild().CreateChild().CreateChild();
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

	if (myEntityHierarchyNeedsUpdating)
	{
		BuildHierarchy();
		myEntityHierarchyNeedsUpdating = false;
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

void GameManager::OnImGui()
{
	ImGui::Separator();
	ImGui::Text("Inspector");
	ImGui::PushStyleColor(ImGuiCol_Button, { .1f, .1f, .1f, 1 });
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
	//if (ImGui::BeginTable("", 2, ImGuiTableFlags_BordersOuter))
	//{
		//ImGui::TableNextRow();
		//ImGui::TableSetColumnIndex(0);
	ImGui::Text("Game Objects");
	ImGui::SameLine();
	AddEntity();
	SelectEntity();
	//ImGui::TableSetColumnIndex(1);
	ImGui::Text("");
	AddEntityComponent();
	//ImGui::EndTable();
//}
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

	if (mySelectedEntity == anEntityID)
	{
		mySelectedEntity = UINT_MAX;
	}

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

	std::set<uint> entityComponents;
	for (auto& [componentID, component] : myEntityComponents[mySelectedEntity])
	{
		entityComponents.insert(componentID);
	}

	ImGui::SameLine();
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
	std::string label = ValidSelection() ? "Add Child Object" : "Add Object";
	if (ImGui::Button(label.c_str()))
	{
		Entity& e = CreateEntity();
		if (ValidSelection())
		{
			GetEntity(mySelectedEntity).AdoptChild(e.GetID());
		}
	}
	if (ValidSelection())
	{
		ImGui::SameLine();
		if (ImGui::Button("Remove"))
		{
			MarkEntityForRemoval(mySelectedEntity);
		};
	}
}

void GameManager::SelectEntity()
{
	// List object hierarchy
	if (ImGui::BeginListBox("", { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y / 3 }))
	{
		ListEntityRecursive(UINT_MAX);
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

void GameManager::ListEntityRecursive(uint anID)
{
	for (auto& id : myEntityHierarchy[anID])
	{
		ImGui::PushID(id);
		ImGuiTreeNodeFlags nodeFlag = ImGuiTreeNodeFlags_Leaf;
		if (mySelectedEntity == id)
		{
			nodeFlag |= ImGuiTreeNodeFlags_Selected;
		}

		bool& showChildren = myShowChildrenRecord[id];
		std::string label = myEntityHierarchy[id].empty() ? "   " : (showChildren ? " - " : " + ");
		if (ImGui::Button(label.c_str()))
		{
			showChildren = !showChildren;
		}
		ImGui::SameLine();
		if (ImGui::TreeNodeEx("Entities", nodeFlag, "%s_%u", "Entity", id))
		{
			if (ImGui::IsItemClicked())
			{
				mySelectedEntity = mySelectedEntity == id ? UINT_MAX : id;
			}
			ImGui::PopID();
			if (showChildren)
			{
				ListEntityRecursive(id);
			}
			ImGui::TreePop();
		}
	}
}

void GameManager::BuildHierarchy()
{
	// Build hierarchy
	myEntityHierarchy.clear();
	for (auto& [id, entity] : myEntities)
	{
		myEntityHierarchy[entity->GetParentID()].insert(id);
		for (auto& childID : entity->GetChildrenIDs())
		{
			myEntityHierarchy[id].insert(childID);
		}
	}
}

void GameManager::UpdateHierarchy()
{
	myEntityHierarchyNeedsUpdating = true;
}

