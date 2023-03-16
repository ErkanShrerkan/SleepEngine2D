#include "pch.h"
#include "Editor.h"
#include <Engine/Input.h>
#include <ImGui/imgui.h>
#include <Engine\Engine.h>
#include "Globals.h"
#include <Engine\DebugProfiler.h>
#include "Entity.h"

Game::Editor::~Editor()
{
}

bool Game::Editor::Init()
{
	Singleton<GlobalSettings>().isEditingMode = true;
	myShowChildrenRecord[UINT_MAX] = true;
	myGM.Init();
	SE::CEngine::GetInstance()->SetGameManagerRef(&myGM);
	return true;
}

bool Game::Editor::Update()
{
	if (Input::GetInputReleased(eInputEvent::F4))
	{
		myIsRunning = false;
	}

	myGM.Update();
	OnImGui();

	return myIsRunning;
}

void Game::Editor::RecieveMessage(eMessage aMessage)
{
	aMessage;
}

void Game::Editor::OnImGui()
{
	ImGui::DockSpaceOverViewport();
	ImGui::PushStyleColor(ImGuiCol_Button, { .1f, .1f, .1f, 1 });
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
	
	Profiler();
	SceneHierarchy();
	Inspector();
	ContentBrowser();
	Controls();

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

void Game::Editor::AddEntityComponent()
{
	if (!ValidSelection())
	{
		return;
	}

	if (ImGui::Button("Reload"))
	{
		if (ValidSelection())
		{
			for (auto& [id, component] : myGM.GetEntityComponents()[mySelectedEntity])
			{
				component->Reload();
			}
		}
	};

	std::set<uint> entityComponents;
	for (auto& [componentID, component] : myGM.GetEntityComponents()[mySelectedEntity])
	{
		entityComponents.insert(componentID);
	}

	ImGui::SameLine();
	ImGui::PushID("Component Combo");
	if (ImGui::BeginCombo("", "Add Component"))
	{
		for (auto& [componentID, map] : myGM.GetComponentMaps())
		{
			// can't add a component that already has been added
			if (entityComponents.find(componentID) != entityComponents.end())
			{
				continue;
			}

			if (ImGui::MenuItem(map->GetName().c_str()))
			{
				map->AddComponent(mySelectedEntity, myGM);
			}
		}
		ImGui::EndMenu();
	}
	ImGui::PopID();
}

void Game::Editor::AddEntity()
{
	std::string label = ValidSelection() ? "Add Child Object" : "Add Object";
	if (ImGui::Button(label.c_str()))
	{
		Entity& e = myGM.CreateEntity();
		if (ValidSelection())
		{
			myGM.GetEntity(mySelectedEntity).AdoptChild(e.GetID());
		}
	}
	if (ValidSelection())
	{
		ImGui::SameLine();
		if (ImGui::Button("Remove"))
		{
			myGM.MarkEntityForRemoval(mySelectedEntity);
			mySelectedEntity = UINT_MAX;
		};
	}
}

void Game::Editor::SelectEntity()
{
	// List object hierarchy
	if (ImGui::BeginListBox("", { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y }))
	{
		ListEntityRecursive(UINT_MAX);
		ImGui::EndListBox();
	}
}

void Game::Editor::ModifyValues()
{
	if (!ValidSelection())
	{
		return;
	}

	// loop components
	auto& components = myGM.GetEntityComponents()[mySelectedEntity];
	for (auto& [id, component] : components)
	{
		{
			ImGui::PushID(id);
			component->OnImGui(myGM.GetComponentMaps()[id]->myName);
			ImGui::PopID();
		}
	}
	ImGui::Separator();
}

void Game::Editor::ListEntityRecursive(uint anID)
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

bool Game::Editor::ValidSelection()
{
	return mySelectedEntity != UINT_MAX;
}

void Game::Editor::BuildHierarchy()
{
	// Build hierarchy
	myEntityHierarchy.clear();
	for (auto& [id, entity] : myGM.GetEntities())
	{
		myEntityHierarchy[entity->GetParentID()].insert(id);
		for (auto& childID : entity->GetChildrenIDs())
		{
			myEntityHierarchy[id].insert(childID);
		}
	}
}

void Game::Editor::SceneHierarchy()
{
	if (myGM.GetUpdateHierarchy())
	{
		myGM.UpdateHierarchy(false);
		BuildHierarchy();
	}

	ImGui::Begin("Scene Hierarchy");
	{
		ImGui::Text("Game Objects");
		ImGui::SameLine();
		AddEntity();
		SelectEntity();
	}
	ImGui::End();
}

void Game::Editor::Inspector()
{
	ImGui::Begin("Inspector");
	{
		AddEntityComponent();
		ModifyValues();
	}
	ImGui::End();
}

void Game::Editor::Profiler()
{
	Singleton<SE::Debug::CDebugProfiler>().Render();
}

void Game::Editor::ContentBrowser()
{
}

void Game::Editor::Controls()
{
	if (ImGui::Begin("Editor", 0,
		//ImGuiWindowFlags_NoScrollbar |
		//ImGuiWindowFlags_NoMove |
		//ImGuiWindowFlags_NoCollapse |
		//ImGuiWindowFlags_NoResize |
		//ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_None
	))
	{
		if (ImGui::Button("Play"))
		{
			myIsRunning = false;
		}
	}
	ImGui::End();
}
