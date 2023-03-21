#include "pch.h"
#include "Editor.h"
#include <Engine/Input.h>
#include <ImGui/imgui.h>
#include <Engine\Engine.h>
#include "Globals.h"
#include <Engine\DebugProfiler.h>
#include "Entity.h"
#include "EditorController.h"
#include "EntityPickingComponent.h"
#include <Engine\TextureFactory.h>
#include <Engine\Texture.h>

Game::Editor::~Editor()
{
}

bool Game::Editor::Init()
{
	myCurrentPath = std::filesystem::relative("Assets");
	Input::SetIsEditing(true);
	myShowChildrenRecord[UINT_MAX] = true;
	myGM.Init();
	myEditorEntityID = myGM.CreateEntity().AddComponent<EditorController>().GameObject().GetID();
	myPicker = myGM.GetEntity(myEditorEntityID).GetComponent<EntityPickingComponent>();
	SE::CEngine::GetInstance()->SetGameManagerRef(&myGM);
	return true;
}

bool Game::Editor::Update()
{
	if (Input::GetInputReleased(eInputEvent::F4))
	{
		myIsRunning = false;
	}

	mySelectedEntityLastFrame = mySelectedEntity;
	if (myClearThumbnails)
	{
		myAssetThumbnails.clear();
		myClearThumbnails = false;
	}
	myGM.UpdateEntityRemoval();
	myGM.UpdateSystems();
	HandleSelection();
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
	//ImGui::PushStyleColor(ImGuiCol_Button, { .1f, .1f, .1f, 1 });
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

	BlackScreen();
	ImGui::ShowDemoWindow();
	Profiler();
	SceneHierarchy();
	Inspector();
	ContentBrowser();
	Controls();
	GameWindow();

	//ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

void Game::Editor::BlackScreen()
{
	ImColor bgCol = { 0.f, 0.f, 0.f, 1.f };
	ImVec2 tl = { 0, 0 };
	uint2 res = Singleton<GlobalSettings>().gameplayResolution;
	ImVec2 br = { (float)res.x, (float)res.y };
	ImGui::GetBackgroundDrawList()->AddRectFilled(tl, br, bgCol);
}

void Game::Editor::AddEntityComponent()
{
	if (!ValidSelection())
	{
		return;
	}

	std::set<uint> entityComponents;
	for (auto& [componentID, component] : myGM.GetEntityComponents()[mySelectedEntity])
	{
		entityComponents.insert(componentID);
	}

	if (ImGui::Button("Reload") && ValidSelection())
	{
		for (auto& [id, component] : myGM.GetEntityComponents()[mySelectedEntity])
		{
			component->Reload();
		}
	}

	ImGui::SameLine();
	ImGui::PushID("Component Combo");
	if (ImGui::BeginCombo("", "Add Component"))
	{
		//ImGui::PushStyleColor()
		for (auto& [componentID, map] : myGM.GetComponentMaps())
		{
			// can't add a component that already has been added or is editor only
			if (map->myIsEditorOnly || (entityComponents.find(componentID) != entityComponents.end()))
				continue;

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

	if (!ValidSelection())
		return;

	ImGui::SameLine();
	if (ImGui::Button("Remove"))
	{
		myGM.MarkEntityForRemoval(mySelectedEntity);
		mySelectedEntity = UINT_MAX;
		myPicker->SetPickedEntityID(UINT_MAX);
	};
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
		if (id == myEditorEntityID)
			continue;

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
				myPicker->SetPickedEntityID(mySelectedEntity);
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

void Game::Editor::HandleSelection()
{
	uint pickedID = myPicker->GetPickedEntityID();
	if (pickedID != mySelectedEntityLastFrame)
	{
		mySelectedEntity = pickedID;
		if (ValidSelection())
		{
			uint id = mySelectedEntity;
			std::vector<uint> trace;
			while (id != UINT_MAX)
			{
				id = myGM.GetEntity(id).GetParentID();
				trace.push_back(id);
			}

			for (auto& parent : trace)
			{
				myShowChildrenRecord[parent] = true;
			}
		}
	}
}

void Game::Editor::LoadThumbnail(const std::string& anImgPath)
{
	if (myAssetThumbnails.find(anImgPath) == myAssetThumbnails.end())
	{
		myAssetThumbnails[anImgPath] = Singleton<SE::CTextureFactory>().LoadTexture(anImgPath);
	}
}

ID3D11ShaderResourceView* const Game::Editor::GetThumbnail(const std::string& anImgPath) const noexcept
{
	return myAssetThumbnails.at(anImgPath)->GetShaderResourceView();
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
	ImGui::Begin("Profiler##Editor");
	Singleton<SE::Debug::CDebugProfiler>().Render();
	ImGui::End();
}

void Game::Editor::ContentBrowser()
{
	ImGui::Begin("Content Browser");
	{
		ImGui::DockSpace(ImGui::GetID("CB"));
		DirectoryNavigator();
		Assets();
	}
	ImGui::End();
}

void Game::Editor::Controls()
{
	ImGui::Begin("Editor", 0,
		//ImGuiWindowFlags_NoScrollbar |
		//ImGuiWindowFlags_NoMove |
		//ImGuiWindowFlags_NoCollapse |
		//ImGuiWindowFlags_NoResize |
		//ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_None
	);
	{
		if (ImGui::Button("Play"))
		{
			myIsRunning = false;
		}
	}
	ImGui::End();
}

void Game::Editor::GameWindow()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::Begin("Game Window"))
	{
		uint2 res = Singleton<GlobalSettings>().gameplayResolution;
		uint2 windowRes = Singleton<GlobalSettings>().windowResolution;
		float ratio = (float)res.x / res.y;
		float frameH = ImGui::GetFrameHeight();
		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		ImVec2 viewPortSize = windowSize;
		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 cursorPos;
		// too wide
		if (windowSize.x * 1.f / ratio > windowSize.y)
		{
			cursorPos = { (windowSize.x - (ratio * windowSize.y)) * 0.5f, frameH };
			viewPortSize.x = ratio * windowSize.y;
		}
		// too narrow
		else
		{
			cursorPos = { 0, frameH + ((windowSize.y - (windowSize.x * 1.f / ratio)) * 0.5f) };
			viewPortSize.y = windowSize.x * 1.f / ratio;
		}
		ImGui::SetCursorPos(cursorPos);
		cursorPos.x += pos.x;
		cursorPos.y += pos.y;
		Singleton<GlobalSettings>().gameWindowRect =
		{
			((cursorPos.x / windowRes.x)),
			((cursorPos.y / windowRes.y)),
			((cursorPos.x + viewPortSize.x) / windowRes.x),
			((cursorPos.y + viewPortSize.y) / windowRes.y),
		};
		float4 rect = Singleton<GlobalSettings>().gameWindowRect;
		ImVec2 tl = { rect.x, rect.y };
		ImVec2 br = { rect.z, rect.w };
		ImColor col = { .2f, .2f, .2f, 1.f };
		ImColor sideBarCol = { .15f, .15f, .15f, 1.f };
		ImVec2 posPlusWindowSize = { pos.x + windowSize.x, pos.y + windowSize.y + frameH };
		ImGui::GetWindowDrawList()->AddRectFilled(pos, posPlusWindowSize, sideBarCol);
		ImGui::Image((void*)Singleton<GlobalSettings>().gameViewTexture->GetSRV(), viewPortSize);
		ImGui::GetWindowDrawList()->AddRect(tl, br, col);
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void Game::Editor::DirectoryNavigator()
{
	ImGui::Begin("Directories");
	{
		/*auto it = std::filesystem::directory_iterator("Assets");

		for (const auto& dir : it)
		{
			if (dir.is_directory())
			{
				ImGui::PushID(ImGui::GetID(dir.path().string().c_str()));
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
						myPicker->SetPickedEntityID(mySelectedEntity);
					}
					ImGui::PopID();
					if (showChildren)
					{
						ListEntityRecursive(id);
					}
					ImGui::TreePop();
				}
			}
		}*/
	}
	ImGui::End();
}

void Game::Editor::Assets()
{
	ImGui::Begin("Assets");
	ImGui::Text(myCurrentPath.string().c_str());
	{
		auto base = std::filesystem::path("Assets");
		if (myCurrentPath != base)
		{
			if (ImGui::Button("  <-  "))
			{
				myCurrentPath = myCurrentPath.parent_path();
				myClearThumbnails = true;
			}
		}

		static float padding = 16;
		static float imgSize = 128;
		float cellSize = padding + imgSize;
		float windowWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(windowWidth / cellSize);

		ImGui::Columns(columnCount > 0 ? columnCount : 1, 0, false);

		auto it = std::filesystem::directory_iterator(myCurrentPath);
		for (const auto& dir : it)
		{
			bool isDir = dir.is_directory();
			const auto& dirPath = dir.path();
			std::string path(dir.path().string());
			std::replace(path.begin(), path.end(), '\\', '/');

			if (isDir && path == "Assets/Textures/Editor")
				continue;
			
			std::string relativePath = std::string(path.substr(path.find_last_of('/') + 1, path.size()));
			std::string fileName = relativePath;

			std::string ext = std::string(path.end() - 4, path.end());
			ImVec2 imgSizeV = { imgSize, imgSize };
			ID3D11ShaderResourceView* srv = nullptr;
			// Set image based on type (can expand later)
			if (isDir)
			{
				// load dir image
				std::string imgPath = "Assets/Textures/Editor/Folder.dds";
				LoadThumbnail(imgPath);
				srv = GetThumbnail(imgPath);
			}
			else if (ext == ".dds")
			{
				LoadThumbnail(path);
				srv = GetThumbnail(path);
			}
			else if (ext == ".png")
			{
				//LoadThumbnail(path);
				srv = nullptr;//GetThumbnail(path);
			}
			else
			{
				// load misc image
				srv = nullptr;
			}

			ImGui::PushID(ImGui::GetID(fileName.c_str()));
			if (ImGui::ImageButton(srv, imgSizeV))
			{
				if (isDir)
				{
					myCurrentPath /= dirPath.filename();
					myClearThumbnails = true;
				}
				else
				{
					// drag drop resource if texture or sumn
					// maybe if file is a scene:
					// save current scene -> load new scene
				}
			}
			ImGui::PopID();
			ImGui::TextWrapped(fileName.c_str());
			ImGui::NextColumn();
		}
		//ImGui::Columns(1);
		//ImGui::SliderFloat("Img Size", &imgSize, 16, 512);
		//ImGui::SliderFloat("Padding", &padding, 0, 128);
	}
	ImGui::End();
}
