#include "pch.h"
#include "Globals.h"
#include "Editor.h"

#include <fstream>

// Engine
#include <Engine\Input.h>
#include <Engine\Engine.h>
#include <Engine\DebugProfiler.h>
#include <Engine\TextureFactory.h>
#include <Engine\GraphicsEngine.h>
#include <Engine\WindowHandler.h>
#include <Engine\Texture.h>
#include <Engine\LineDrawer.h>

// ECS
#include "Entity.h"
#include "EditorController.h"
#include "EntityPickingComponent.h"
#include "Transform.h"
#include "CameraComponent.h"

// ImGui
#include <ImGui\imgui.h>

// ImGuizmo
#include <ImGuizmo\ImGuizmo.h>

Game::Editor::~Editor()
{
	StopObservingAllEvents();
}

bool Game::Editor::Init()
{
	Input::SetIsEditing(true);
	myGM.Init();
	SE::CEngine::GetInstance()->SetGameManagerRef(&myGM);
	myIsObservingEditorInputs = true;

	myShowChildrenRecord[ENTITY_HIERARCHY_ROOT] = true;
	myCurrentPath = std::filesystem::relative("Assets");
	myEditorEntityID = myGM.CreateEntity().AddComponent<EditorController>().GameObject().GetID();
	myPicker = myGM.GetEntity(myEditorEntityID).GetComponent<EntityPickingComponent>();

	eInputState state = eInputState::Pressed;
	ObserveInputEvent(eInputEvent::Q, state, [&]() { this->SetTransformOperation(eTransformOperation::None); });
	ObserveInputEvent(eInputEvent::W, state, [&]() { this->SetTransformOperation(eTransformOperation::Translate); });
	ObserveInputEvent(eInputEvent::E, state, [&]() { this->SetTransformOperation(eTransformOperation::Rotate); });
	ObserveInputEvent(eInputEvent::R, state, [&]() { this->SetTransformOperation(eTransformOperation::Scale); });

	return true;
}

bool Game::Editor::Update()
{
	myIsRunning = !Input::GetInputReleased(eInputEvent::F4);

	InternalUpdate();

	return myIsRunning;
}

void Game::Editor::RecieveMessage(eMessage aMessage)
{
	aMessage;
}

void Game::Editor::OnImGui()
{
	ImGui::DockSpaceOverViewport();
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

	CheckEnableEditorControls();
	BlackScreen();
	HandleSelection();
	EditorDockSpace();
	ImGui::ShowDemoWindow();
	Profiler();
	SceneHierarchy();
	Inspector();
	ContentBrowser();
	Controls();
	GameWindow();
	DrawWorldGrid();

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
	auto& ec = myGM.GetEntityComponents()[mySelectedEntity];
	for (auto& [componentID, component] : ec)
	{
		entityComponents.insert(componentID);
	}

	ImGui::Text("Active");
	ImGui::SameLine();
	ImGui::PushID("Active Box");
	ImGui::Checkbox("", &myGM.GetEntity(mySelectedEntity).GetActiveRef());
	ImGui::PopID();
	ImGui::SameLine();

	if (ImGui::Button("Reload") && ValidSelection())
	{
		for (auto& [id, component] : ec)
		{
			component->Reload();
		}
	}

	ImGui::SameLine();

	ImGui::PushID("Component Combo");
	ImGui::SetNextItemWidth(ImGui::CalcTextSize("Add Component		").x);
	if (ImGui::BeginCombo("", "Add Component"))
	{
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
		mySelectedEntity = NULL_ENTITY;
		myPicker->SetPickedEntityID(NULL_ENTITY);
	};
}

void Game::Editor::SelectEntity()
{
	// List object hierarchy
	if (ImGui::BeginListBox("", { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y }))
	{
		ListEntityRecursive(ENTITY_HIERARCHY_ROOT);
		ImGui::EndListBox();
	}
}

void Game::Editor::ModifyValues()
{
	if (!ValidSelection())
		return;

	// loop components
	auto& components = myGM.GetEntityComponents()[mySelectedEntity];
	for (auto& [id, component] : components)
	{
		ImGui::PushID(id);
		auto& map = myGM.GetComponentMaps()[id];
		auto* exposer = component->GetComponentExposer();
		exposer->OnImGuiBegin(map->myName);
		
		uint entityID = mySelectedEntity;
		UpdateDragAndDrop([&]()
			{
				ImGui::SetDragDropPayload("DRAG_COMPONENT_REF", &component, sizeof(Component*));
				ImGui::Text("Ref %s", std::to_string(entityID).c_str());
				ImGui::EndDragDropSource();
			}, id);

		exposer->OnImGui();

		ImGui::PopID();
	}
	ImGui::Separator();
}

void Game::Editor::ListEntityRecursive(uint anID)
{
	for (auto& id : myEntityHierarchy[anID])
	{
		//if (id == myEditorEntityID)
		//	continue;

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
		// change to display entity name in future
		if (ImGui::TreeNodeEx("Entities", nodeFlag, "%s_%u", "Entity", id))
		{
			bool hovered = ImGui::IsItemHovered();
			HandleHierarchySelection(id, hovered);

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
	return mySelectedEntity != INVALID_ENTITY;
}

void Game::Editor::CheckEnableEditorControls()
{
	myGM.GetEntity(myEditorEntityID).SetActive(myGameWindowActive);
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
	mySelectedEntityLastFrame = mySelectedEntity;

	uint pickedID = myPicker->GetPickedEntityID();
	if (pickedID == mySelectedEntityLastFrame)
		return;

	if (ImGuizmo::IsOver() || ImGuizmo::IsUsing())
	{
		myPicker->SetPickedEntityID(mySelectedEntity);
		return;
	}

	mySelectedEntity = pickedID;

	if (!ValidSelection())
		return;

	uint id = mySelectedEntity;
	std::vector<uint> trace;
	while (id != INVALID_ENTITY)
	{
		id = myGM.GetEntity(id).GetParentID();
		trace.push_back(id);
	}

	for (auto& parent : trace)
	{
		myShowChildrenRecord[parent] = true;
	}

	//myPicker->SetPickedEntityID(NULL_ENTITY);
}

void Game::Editor::InvalidateSelectionIfInvalid()
{
	if (!ValidSelection())
		return;

	auto& entities = myGM.GetEntities();
	auto it = entities.find(mySelectedEntity);
	if (it == entities.end())
	{
		mySelectedEntity = INVALID_ENTITY;
	}
}

void Game::Editor::SetTransformOperation(eTransformOperation anOperation)
{
	myOperation = anOperation;
}

void Game::Editor::SetTransformSpace(eTransformSpace aSpace)
{
	mySpace = aSpace;
}

void Game::Editor::UpdateDragAndDrop(std::function<void()> aLoadPayloadFunc, uint anID)
{
	if (ImGui::IsItemHovered())
	{
		if (Input::GetInputReleased(eInputEvent::LMB))
		{
			if (!myHoversInitiallySelectedItem)
			{
				myInitiallySelectedItem = NULL_ENTITY;
			}
			else
			{
				mySelectedItem = mySelectedItem == myInitiallySelectedItem ? NULL_ENTITY : myInitiallySelectedItem;
			}
		}
		else if (Input::GetInputPressed(eInputEvent::LMB))
		{
			myInitiallySelectedItem = anID;
			myHoversInitiallySelectedItem = true;
		}
	}
	else
	{
		if (myInitiallySelectedItem == anID)
		{
			myInitiallySelectedItem = NULL_ENTITY;
			myHoversInitiallySelectedItem = false;

			// handle drag n drop
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				aLoadPayloadFunc();
			}
		}
	}
}

void Game::Editor::RegisterNewProjectFile(
	tinyxml2::XMLElement* anElement,
	const std::string& aType,
	const std::string& aFileName)
{
	tinyxml2::XMLElement* element = anElement->FirstChildElement("ItemGroup");
	tinyxml2::XMLElement* type = nullptr;
	bool success = false;
	for (size_t i = 0; i < 10; i++)
	{
		type = element->FirstChildElement(aType.c_str());
		if (type)
		{
			type = element->InsertNewChildElement(aType.c_str());
			success = true;
			break;
		}
		element = element->NextSiblingElement("ItemGroup");
	}

	if (!success)
		return;

	type->SetAttribute("Include", aFileName.c_str());
	element->InsertEndChild(type);
}

void Game::Editor::RegisterFileToFilter(
	tinyxml2::XMLElement* anElement,
	const std::string& aType,
	const std::string& aFileName,
	const std::string& aFilter)
{
	tinyxml2::XMLElement* element = anElement->FirstChildElement("ItemGroup");
	tinyxml2::XMLElement* type = nullptr;
	bool success = false;
	for (size_t i = 0; i < 10; i++)
	{
		type = element->FirstChildElement(aType.c_str());
		if (type)
		{
			type = element->InsertNewChildElement(aType.c_str());
			success = true;
			break;
		}
		element = element->NextSiblingElement("ItemGroup");
	}

	if (!success)
		return;

	type->SetAttribute("Include", aFileName.c_str());
	tinyxml2::XMLElement* filterElement = type->InsertNewChildElement("Filter");
	std::string filter = "Source Files" + (aFilter.empty() ? "" : "\\" + aFilter);
	filterElement->SetText(filter.c_str());
	element->InsertEndChild(type);
}

void Game::Editor::LoadXMLFile(tinyxml2::XMLDocument& aDoc, const std::string& aPath)
{
	std::ifstream inputFile(aPath);
	if (!inputFile.is_open())
	{
		printe("ERROR LOADING PROJ FILE\n");
		return;
	}
	std::string line;
	std::string xmlContent;
	while (getline(inputFile, line))
	{
		xmlContent += line;
	}
	inputFile.close();
	if (aDoc.Parse(xmlContent.c_str()) != tinyxml2::XML_SUCCESS)
	{
		printe("ERROR PARSING PROJ FILE\n");
		return;
	}
}

void Game::Editor::SaveXMLFile(tinyxml2::XMLDocument& aDoc, const std::string& aPath)
{
	std::ofstream outputFile(aPath);
	if (!outputFile.is_open())
	{
		printe("ERROR WRITING PROJ FILE\n");
		return;
	}
	tinyxml2::XMLPrinter printer;
	aDoc.Print(&printer);
	outputFile << printer.CStr();
	outputFile.close();
}

void Game::Editor::WriteTextFile(const std::string& aPath, const std::string& someContent)
{
	std::ofstream file(aPath);
	file << someContent;
	file.close();
}

void Game::Editor::AppendLineToTextFile(const std::string& aPath, const std::string& someText)
{
	std::ifstream finput;
	finput.open(aPath);

	if (!finput.is_open())
	{
		printe("Failed to open file %s for reading", aPath.c_str());
		return;
	}

	std::ofstream foutput;
	foutput.open(aPath, std::ios::app);
	if (!finput.is_open())
	{
		finput.close();
		printe("Failed to open file %s for writing", aPath.c_str());
		return;
	}

	foutput << "\n" + someText;

	printe("Text append to %s [SUCCESS]\n", aPath.c_str());

	finput.close();
	foutput.close();
}

bool Game::Editor::FileExists(const std::string& aFilePath)
{
	return std::filesystem::exists(aFilePath.c_str());
}

void Game::Editor::HandleHierarchySelection(uint anID, bool isHovered)
{
	if (!isHovered)
	{
		if (myInitiallySelectedEntity == anID)
		{
			myInitiallySelectedEntity = NULL_ENTITY;
			myHoversInitiallySelectedEntity = false;

			// handle drag n drop
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
			{
				ImGui::SetDragDropPayload("DRAG_ENTITY_REF", &anID, sizeof(uint));
				ImGui::Text("Ref %s", std::to_string(anID).c_str());
				ImGui::EndDragDropSource();
			}
		}
		return;
	}

	if (Input::GetInputReleased(eInputEvent::LMB))
	{
		if (!myHoversInitiallySelectedEntity)
		{
			myInitiallySelectedEntity = NULL_ENTITY;
			return;
		}

		mySelectedEntity = mySelectedEntity == myInitiallySelectedEntity ? NULL_ENTITY : myInitiallySelectedEntity;
		myPicker->SetPickedEntityID(mySelectedEntity);
	}
	else if (Input::GetInputPressed(eInputEvent::LMB))
	{

		myInitiallySelectedEntity = anID;
		myHoversInitiallySelectedEntity = true;
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

void Game::Editor::CheckClearThumbnails()
{
	if (myClearThumbnails)
	{
		myAssetThumbnails.clear();
		myClearThumbnails = false;
	}
}

void Game::Editor::InternalUpdate()
{
	CheckClearThumbnails();

	myGM.UpdateEntityRemoval();
	myGM.UpdateSystems();
	//myGM.UpdateComponents();

	InvalidateSelectionIfInvalid();

	OnImGui();
}

void Game::Editor::DrawWorldGrid()
{
	float meter = 100;
	float2 res = { 100 * meter, 100 * meter };
	for (float x = -res.x; x < res.x; x += meter)
	{
		Debug::DrawLine2D({ x, -res.x }, { x, res.x }, { 1, 1, 1, .125f });
	}
	for (float y = -res.y; y < res.y; y += meter)
	{
		Debug::DrawLine2D({ -res.y, y }, { res.y, y }, { 1, 1, 1, .125f });
	}

	Debug::DrawCircle({ 0.f, 0.f }, 50);
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
		ImGui::Text("Scene");
		ImGui::SameLine();
		if (ImGui::Button("Save"))
		{
			myGM.SaveScene();
		}

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
			myGM.SaveScene();
		}
	}
	ImGui::End();
}

void Game::Editor::GameWindow()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::Begin("Game Window"))
	{
		myGameWindowActive = ImGui::IsWindowFocused();
		RenderViewport();
		RenderGizmos();
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
		float padding = 16;
		float imgSize = 128;
		float cellSize = padding + imgSize;
		float windowWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(windowWidth / cellSize);
		ImVec2 imgSizeV = { imgSize, imgSize };

		ImGui::Columns(columnCount > 0 ? columnCount : 1, 0, false);

		if (myCurrentPath != base)
		{
			ImGui::PushID("Back Button");
			if (ImGui::Button("BACK", imgSizeV))
			{
				myCurrentPath = myCurrentPath.parent_path();
				myClearThumbnails = true;
			}
			ImGui::PopID();
			ImGui::NextColumn();
		}

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
			ID3D11ShaderResourceView* srv = nullptr;
			// Set image based on type (can expand later)
			if (isDir)
			{
				// load dir image
				std::string imgPath = "Assets/Textures/Editor/Folder.dds";
				LoadThumbnail(imgPath);
				srv = GetThumbnail(imgPath);
			}
			else if (ext == ".dds" || ext == ".DDS")
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

			uint id = ImGui::GetID(fileName.c_str());
			ImGui::PushID(id);
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

			if (isDir)
			{
				ImGui::PopID();
				ImGui::TextWrapped(fileName.c_str());
				ImGui::NextColumn();
				continue;
			}

			UpdateDragAndDrop([&]()
				{
					myActivePayload.SetString(fileName);
					ImGui::SetDragDropPayload("DRAG_FILENAME", &myActivePayload, sizeof(char) * 128);
					ImGui::Text(fileName.c_str());
					ImGui::EndDragDropSource();
				}, id);

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

void Game::Editor::DebugDrawRect()
{
	auto rect = Singleton<GlobalSettings>().windowRect;
	ImVec2 tl = { 0, 0 };
	ImVec2 br = { rect.z - rect.x, rect.w - rect.y };
	ImColor col = { 1.f, 0.f, 0.f, 1.f };
	ImGui::GetForegroundDrawList()->AddRect(tl, br, col);
}

void Game::Editor::DebugDrawMousePos()
{
	auto pos = Input::GetMousePixelPos();
	auto rect = Singleton<GlobalSettings>().windowRect;
	pos -= {rect.x, rect.y};
	ImVec2 tl = { pos.x + 15, pos.y + 15 };
	ImVec2 br = { pos.x, pos.y };
	ImColor col = { 1.f, 0.f, 0.f, 1.f };
	ImGui::GetForegroundDrawList()->AddRectFilled(tl, br, col);
}

void Game::Editor::EditorDockSpace()
{
	ImGui::Begin("Sleep Engine 2D", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);
	{
		ImGui::DockSpace(ImGui::GetID("SE2D"));
		//DebugDrawRect();
		//DebugDrawMousePos();
		if (ImGui::BeginMenuBar())
		{
			ToolsMenu();
			ImGui::EndMenuBar();
		}
	}
	ImGui::End();
}

void Game::Editor::RenderViewport()
{
	float2 viewportSize = CalculateGameWindowRect();
	RenderGameTextureToRect(viewportSize);
}

void Game::Editor::RenderGizmos()
{
	ImGuizmo::SetOrthographic(true);
	ImGuizmo::SetDrawlist();

	auto& gs = Singleton<GlobalSettings>();
	float4 windowRect = gs.windowRect;

	float2 viewPortCenter = gs.GetGameWindowCenterPixel();
	float2 windowCenter = gs.GetWindowCenterPixel();
	float2 viewPortCenterOffset = viewPortCenter - windowCenter;

	float4 gameRect = gs.gameWindowRect;
	gameRect += {
		windowRect.xy,
			windowRect.xy
	};

	float multx = 1.f - (454.f / 1351.f);
	float multy = 1.f - (255.f / 758.f);
	viewPortCenterOffset.x *= multx;
	viewPortCenterOffset.y *= multy;

	float4 gizmoRect = windowRect;

	gizmoRect +=
	{
		viewPortCenterOffset,
			viewPortCenterOffset
	};

	gizmoRect -=
	{
		windowRect.xy,
			windowRect.xy
	};

	ImGuizmo::SetRect(gizmoRect.x, gizmoRect.y, gizmoRect.z, gizmoRect.w);
	ImVec2 tl = { gizmoRect.x, gizmoRect.y };
	ImVec2 br = { gizmoRect.z, gizmoRect.w };

	CameraComponent cam = *myGM.GetComponent<CameraComponent>(myEditorEntityID);
	float4x4 cameraTransform = myGM.GetComponent<Transform>(myEditorEntityID)->GetTransform();
	float4x4 cameraView = float4x4::GetFastInverse(cameraTransform);
	float4x4 cameraProjection = cam.GetProjection();

	if (!ValidSelection())
		return;

	ImGuizmo::OPERATION op = ImGuizmo::UNIVERSAL;
	switch (myOperation)
	{
	case eTransformOperation::None:
		return;
	case eTransformOperation::Scale:
		op = ImGuizmo::SCALE;
		break;
	case eTransformOperation::Rotate:
		op = ImGuizmo::ROTATE;
		break;
	case eTransformOperation::Translate:
		op = ImGuizmo::TRANSLATE;
		break;
	default:
		break;
	}

	Transform& transform = *myGM.GetComponent<Transform>(mySelectedEntity);

	// if no parent then identity
	float4x4 tParentWorld = transform.GetParentWorldSpaceTransform();
	float4x4 tWorld = transform.GetObjectSpaceTransform() * tParentWorld;
	static float4x4 tParentWorldInverseTranspose;
	static float4x4 tNewWorld;
	static Transform unalteredTransform;

	if (!myIsTransforming)
	{
		tNewWorld = tWorld;
	}

	ImGuizmo::Manipulate(
		cameraView.Raw(),
		cameraProjection.Raw(),
		op,
		ImGuizmo::WORLD,
		tNewWorld.Raw(),
		NULL
	);

	if (!ImGuizmo::IsUsing())
	{
		myIsTransforming = false;
		myGM.GetEntity(myEditorEntityID).GetComponent<EditorController>()->myIsObservingInputs = true;
		return;
	}

	if (!myIsTransforming)
	{
		// Just started transformation
		myIsTransforming = true;
		myGM.GetEntity(myEditorEntityID).GetComponent<EditorController>()->myIsObservingInputs = false;
		tParentWorldInverseTranspose = float4x4::Transpose(tParentWorld.Inverse());
		tNewWorld = tWorld;
		unalteredTransform = transform;
		return;
	}

	//Debug::DrawTransform(tNewWorld);
	transform.SetTransform(tNewWorld * tParentWorldInverseTranspose);
}

void Game::Editor::ToolsMenu()
{
	struct ToolMenu
	{
		std::string action;
		std::string name;
		std::function<void()> functionality;
		bool open = false;
	};

	static std::vector<ToolMenu> tools =
	{
		{ "New Component", "Component Generator", [&]() { GenerateComponent(); }},
		{ "New System", "System Generator", [&]() { GenerateSystem(); }},
		{ "New Material", "Material Generator", [&]() { GenerateMaterial(); }}
	};

	if (ImGui::BeginMenu("Tools"))
	{
		for (auto& tool : tools)
		{
			ImGui::MenuItem(tool.action.c_str(), NULL, &tool.open);
		}
		ImGui::EndMenu();
	}

	for (auto& tool : tools)
	{
		if (!tool.open)
			continue;

		ImGui::Begin(tool.name.c_str(), &tool.open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
		tool.functionality();
		ImGui::End();
	}
}

void Game::Editor::GenerateSystem()
{
	static DynamicStringBuffer systemName(32);
	ImGui::Text("System Name");
	ImGui::InputText("##name", systemName[0], systemName.GetSize());

	if (!ImGui::Button("Create System"))
		return;

	std::string name = systemName.GetString();

	if (name.empty())
		return;

	systemName.SetString("");

	std::string path = "../Source/Game/";
	std::string headerFileName = name + ".h";
	std::string sourceFileName = name + ".cpp";

	if (FileExists(path + headerFileName) || FileExists(path + sourceFileName))
	{
		printe("System %s [FAILURE]\nAlready exists\n", name.c_str());
		return;
	}

	std::string headerContent = R"DELIM(#pragma once
#include "System.h"

class )DELIM" + name + R"DELIM( : public System
{
public:
	)DELIM" + name + R"DELIM((GameManager* aGM)
		: System::System(aGM)
	{

	}

	virtual void Update() override;
};
)DELIM";

	std::string sourceContent = R"DELIM(#include "pch.h"
#include ")DELIM" + headerFileName + R"DELIM("
#include "GameManager.h"

void )DELIM" + name + R"DELIM(::Update()
{
	// TODO: Implement )DELIM" + name + R"DELIM(::Update()
}
)DELIM";

	WriteTextFile(path + headerFileName, headerContent);
	WriteTextFile(path + sourceFileName, sourceContent);

	// add files to project
	std::string projPath = "../Source/Game/Game.vcxproj";
	tinyxml2::XMLDocument doc;
	LoadXMLFile(doc, projPath);
	tinyxml2::XMLElement* root = doc.RootElement();
	RegisterNewProjectFile(root, "ClInclude", headerFileName);
	RegisterNewProjectFile(root, "ClCompile", sourceFileName);
	SaveXMLFile(doc, projPath);

	// Add files to filter
	std::string filtersPath = projPath + ".filters";
	LoadXMLFile(doc, filtersPath);
	root = doc.RootElement();
	RegisterFileToFilter(root, "ClInclude", headerFileName, "Game\\ECS\\Systems");
	RegisterFileToFilter(root, "ClCompile", sourceFileName, "Game\\ECS\\Systems");
	SaveXMLFile(doc, filtersPath);

	AppendLineToTextFile(path + "SystemRegister.h", "RegisterSystem<" + name + ">();");
	AppendLineToTextFile(path + "SystemInclude.h", "#include \"" + headerFileName + "\"");

	printe("System %s [SUCCESS]\n", name.c_str());
}

void Game::Editor::GenerateComponent()
{
	// Might make tool for deleting systems and components 
	// that cleans up proj and filter files but its low prio

	static bool initWithUpdate = false;
	static bool isEditorOnly = false;
	static DynamicStringBuffer componentName(32);
	static DynamicStringBuffer componentDisplayName(32);

	ImGui::Text("Component Name");
	ImGui::InputText("##name", componentName[0], componentName.GetSize());
	ImGui::Text("Component Display Name");
	ImGui::InputText("##displayname", componentDisplayName[0], componentDisplayName.GetSize());
	ImGui::Checkbox("Init With Update", &initWithUpdate);
	ImGui::Checkbox("Is Editor Only", &isEditorOnly);

	if (!ImGui::Button("Create Component"))
		return;

	std::string name = componentName.GetString();
	std::string displayName = componentDisplayName.GetString();

	if (name.empty() || displayName.empty())
		return;

	componentName.SetString("");
	componentDisplayName.SetString("");

	std::string path = "../Source/Game/";
	std::string headerFileName = name + ".h";
	std::string sourceFileName = name + ".cpp";

	if (FileExists(path + headerFileName) || FileExists(path + sourceFileName))
	{
		printe("Component %s [FAILURE]\nAlready exists\n", name.c_str());
		return;
	}

	std::string updateDeclaration = initWithUpdate ? "\n\tvirtual void Update() override;" : "";
	std::string updateDefinition = initWithUpdate ? "\nvoid " + name + "::Update()\n{\n}\n" : "";
	std::string headerContent = R"DELIM(#pragma once
#include "Component.h"

class )DELIM" + name + R"DELIM( : public Component
{
public:
	)DELIM" + name + R"DELIM(();
	~)DELIM" + name + R"DELIM(();

public:
	virtual void Start() override;)DELIM" + updateDeclaration + R"DELIM(
};
)DELIM";

	std::string sourceContent = R"DELIM(#include "pch.h"
#include ")DELIM" + headerFileName + R"DELIM("

)DELIM" + name + R"DELIM(::)DELIM" + name + R"DELIM(()
{
}

)DELIM" + name + R"DELIM(::~)DELIM" + name + R"DELIM(()
{
}

void )DELIM" + name + R"DELIM(::Start()
{
}
)DELIM" + updateDefinition;

	WriteTextFile(path + headerFileName, headerContent);
	WriteTextFile(path + sourceFileName, sourceContent);

	// add files to project
	std::string projPath = "../Source/Game/Game.vcxproj";
	tinyxml2::XMLDocument doc;
	LoadXMLFile(doc, projPath);
	tinyxml2::XMLElement* root = doc.RootElement();
	RegisterNewProjectFile(root, "ClInclude", headerFileName);
	RegisterNewProjectFile(root, "ClCompile", sourceFileName);
	SaveXMLFile(doc, projPath);

	// Add files to filter
	std::string filtersPath = projPath + ".filters";
	LoadXMLFile(doc, filtersPath);
	root = doc.RootElement();
	RegisterFileToFilter(root, "ClInclude", headerFileName, "Game\\ECS\\Components");
	RegisterFileToFilter(root, "ClCompile", sourceFileName, "Game\\ECS\\Components");
	SaveXMLFile(doc, filtersPath);

	AppendLineToTextFile(path + "ComponentRegister.h", "RegisterComponent<" + name + ">(\"" + displayName + "\"" + (isEditorOnly ? ", true" : "") + "); ");
	AppendLineToTextFile(path + "ComponentInclude.h", "#include \"" + headerFileName + "\"");

	printe("Component %s [SUCCESS]\n", name.c_str());
}

void Game::Editor::GenerateMaterial()
{
}

float2 Game::Editor::CalculateGameWindowRect()
{
	auto& gs = Singleton<GlobalSettings>();

	uint2 res = gs.gameplayResolution;
	float4 windowRect = gs.windowRect;
	uint2 windowRes = {
		uint(windowRect.z - windowRect.x),
		uint(windowRect.w - windowRect.y)
	};

	// game window rect
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 pos = ImGui::GetWindowPos();
	float frameH = ImGui::GetFrameHeight();
	pos.y += frameH;
	gs.gameWindowRect =
	{
		pos.x,
		pos.y,
		pos.x + windowSize.x,
		pos.y + windowSize.y - frameH
	};

	ImVec2 viewportSize =
	{
		gs.gameWindowRect.z - gs.gameWindowRect.x,
		gs.gameWindowRect.w - gs.gameWindowRect.y
	};

	float halfWidthNormal = (float)(uint(gs.gameWindowRect.z - gs.gameWindowRect.x)) / (gs.windowResolution.x * 2);
	float halfHeightNormal = (float)(uint(gs.gameWindowRect.w - gs.gameWindowRect.y)) / (gs.windowResolution.y * 2);

	gs.gameViewRect =
	{
		.5f - halfWidthNormal,
		.5f - halfHeightNormal,
		.5f + halfWidthNormal,
		.5f + halfHeightNormal
	};

	gs.gameScreenRect = gs.gameWindowRect + float4(
		windowRect.x,
		windowRect.y,
		windowRect.x,
		windowRect.y
	);

	return *reinterpret_cast<float2*>(&viewportSize);
}

void Game::Editor::RenderGameTextureToRect(float2 aviewportSize)
{
	ImColor col = { .2f, .2f, .2f, 1.f };
	ImColor sideBarCol = { .15f, .15f, .15f, 1.f };

	auto& gs = Singleton<GlobalSettings>();
	ImVec2 tl = { gs.gameWindowRect.x, gs.gameWindowRect.y };
	ImVec2 br = { gs.gameWindowRect.z, gs.gameWindowRect.w };

	ImVec2 viewportSize = { aviewportSize.x, aviewportSize.y };

	ImGui::GetWindowDrawList()->AddRectFilled(
		tl,			   // min
		br,				// max
		sideBarCol         // color
	);

	// DEBUG game window rect
	ImGui::GetOverlayDrawList()->AddRect(
		tl, // min
		br, // max
		ImColor(1.f, 0.f, 0.f, 1.f) // color
	);

	ImGui::Image(
		(void*)gs.gameViewTexture->GetSRV(),
		viewportSize,
		ImVec2(gs.gameViewRect.x, gs.gameViewRect.y), // uv start
		ImVec2(gs.gameViewRect.z, gs.gameViewRect.w) // uv end
		//ImVec4(1, 1, 1, 1) // tint color
		//ImVec4(1, 0, 0, 1) // border color
	);

	ImGui::GetWindowDrawList()->AddRect(tl, br, col);
}

void Game::Editor::RenderLinesAcrossRect(float4 aRect)
{
	auto& gs = Singleton<GlobalSettings>();
	float4 windowRect = gs.windowRect;

	float4 rect = aRect -
		float4{
		windowRect.xy,
			windowRect.xy
	};
	ImGui::GetOverlayDrawList()->AddLine(
		ImVec2(rect.x, rect.y),			   // min
		ImVec2(rect.z, rect.w),				// max
		ImColor(255, 222, 222, 255)         // color
	);

	ImGui::GetOverlayDrawList()->AddLine(
		ImVec2(rect.x, rect.w),			   // min
		ImVec2(rect.z, rect.y),				// max
		ImColor(255, 222, 222, 255)         // color
	);
}
