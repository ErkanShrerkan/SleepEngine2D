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

	if (ImGui::Button("Reload") && ValidSelection())
	{
		for (auto& [id, component] : ec)
		{
			component->Reload();
		}
	}

	ImGui::SameLine();
	ImGui::PushID("Component Combo");
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
	{
		return;
	}

	// loop components
	auto& components = myGM.GetEntityComponents()[mySelectedEntity];
	for (auto& [id, component] : components)
	{
		{
			ImGui::PushID(id);
			component->GetComponentExposer()->OnImGui(myGM.GetComponentMaps()[id]->myName);
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

void Game::Editor::RegisterNewProjectFile(
	tinyxml2::XMLElement* anElement,
	const std::string& aCategory,
	const std::string& aType,
	const std::string& anAttribute,
	const std::string& aFileName)
{
	tinyxml2::XMLElement* element = anElement->FirstChildElement(aCategory.c_str());
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
		element = element->NextSiblingElement(aCategory.c_str());
	}

	if (!success)
		return;

	type->SetAttribute(anAttribute.c_str(), aFileName.c_str());
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

void Game::Editor::GenerateSystem()
{
	static DynamicStringBuffer systemName(32);
	ImGui::InputText("System Name", systemName[0], systemName.GetSize());

	if (!ImGui::Button("Create System"))
		return;

	std::string name = systemName.GetString();

	if (name.empty())
		return;

	systemName.SetString("");

	std::string path = "../Source/Game/";
	std::string headerFileName = name + ".h";
	std::string sourceFileName = name + ".cpp";

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
};
)DELIM";

	WriteTextFile(path + headerFileName, headerContent);
	WriteTextFile(path + sourceFileName, sourceContent);

	// add files to project
	std::string projPath = "../Source/Game/Game.vcxproj";
	tinyxml2::XMLDocument doc;
	LoadXMLFile(doc, projPath);
	tinyxml2::XMLElement* root = doc.RootElement();
	RegisterNewProjectFile(root, "ItemGroup", "ClInclude", "Include", headerFileName);
	RegisterNewProjectFile(root, "ItemGroup", "ClCompile", "Include", sourceFileName);
	SaveXMLFile(doc, projPath);

	// Add files to filter
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
			SystemGenerator();
			ImGui::EndMenuBar();
		}
	}
	ImGui::End();
}

void Game::Editor::SystemGenerator()
{
	if (ImGui::BeginMenu("Tools"))
	{
		ImGui::MenuItem("Generate Systems", NULL, &myShowSystemGenerator);
		ImGui::EndMenu();
	}

	if (!myShowSystemGenerator)
		return;

	ImGui::Begin("System Generator", &myShowSystemGenerator, ImGuiWindowFlags_NoCollapse);
	GenerateSystem();
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
	float4 rect = gs.gameWindowRect;
	float w = rect.z - rect.x;
	float h = rect.w - rect.y;

	ImGuizmo::SetRect(rect.x, rect.y, w, h);
	ImVec2 tl = { rect.x, rect.y };
	ImVec2 br = { rect.z, rect.w };
	//ImGui::GetWindowDrawList()->AddRect(tl, br, ImColor(1.f, .5f, .125f, 1.f));

	CameraComponent& cam = *myGM.GetComponent<CameraComponent>(myEditorEntityID);
	float* cameraView = float4x4::GetFastInverse(myGM.GetComponent<Transform>(myEditorEntityID)->GetTransform()).Raw();
	float* cameraProjection = cam.GetProjection().Raw();

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
		cameraView,
		cameraProjection,
		op,
		ImGuizmo::WORLD,
		tNewWorld.Raw(),
		NULL
	);

	if (!ImGuizmo::IsUsing())
	{
		myIsTransforming = false;
		return;
	}

	if (!myIsTransforming)
	{
		// Just started transformation
		myIsTransforming = true;
		tParentWorldInverseTranspose = float4x4::Transpose(tParentWorld.Inverse());
		tNewWorld = tWorld;
		unalteredTransform = transform;
		return;
	}

	//Debug::DrawTransform(tNewWorld);
	transform.SetTransform(tNewWorld * tParentWorldInverseTranspose);
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

	//float ratio = (float)res.x / res.y;
	//ImVec2 windowSize = ImGui::GetContentRegionAvail();
	ImVec2 viewportSize =
	{
		gs.gameWindowRect.z - gs.gameWindowRect.x,
		gs.gameWindowRect.w - gs.gameWindowRect.y
	};
	//ImVec2 cursorPos;
	//// too wide
	//if (windowSize.x * 1.f / ratio > windowSize.y)
	//{
	//	cursorPos = { (windowSize.x - (ratio * windowSize.y)) * 0.5f, frameH };
	//	viewportSize.x = ratio * windowSize.y;
	//}
	//// too narrow
	//else
	//{
	//	cursorPos = { 0, frameH + ((windowSize.y - (windowSize.x * 1.f / ratio)) * 0.5f) };
	//	viewportSize.y = windowSize.x * 1.f / ratio;
	//}
	//ImGui::SetCursorPos(cursorPos);
	//cursorPos.x += pos.x;
	//cursorPos.y += pos.y;
	//gs.gameWindowRect =
	//{
	//	cursorPos.x,
	//	cursorPos.y,
	//	cursorPos.x + viewportSize.x,
	//	cursorPos.y + viewportSize.y,
	//};

	return *reinterpret_cast<float2*>(&viewportSize);
}

void Game::Editor::RenderGameTextureToRect(float2 aviewportSize)
{
	ImColor col = { .2f, .2f, .2f, 1.f };
	ImColor sideBarCol = { .15f, .15f, .15f, 1.f };

	auto& gs = Singleton<GlobalSettings>();
	float4 windowRect = gs.windowRect;
	float4 rect = gs.gameWindowRect;
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 pos = ImGui::GetWindowPos();
	float frameH = ImGui::GetFrameHeight();
	pos.y += frameH;

	ImVec2 tl = { rect.x, rect.y };
	ImVec2 br = { rect.z, rect.w };
	ImVec2 posPlusWindowSize = { pos.x + windowSize.x, pos.y + windowSize.y - frameH };
	ImVec2 viewportSize = { aviewportSize.x, aviewportSize.y };

	ImGui::GetWindowDrawList()->AddRectFilled(
		pos,			   // min
		posPlusWindowSize, // max
		sideBarCol         // color
	);

	// DEBUG game window rect
	ImGui::GetOverlayDrawList()->AddRect(
		pos,			   // min
		posPlusWindowSize, // max
		ImColor(1.f, 0.f, 0.f, 1.f) // color
	);

	ImGui::Image(
		(void*)gs.gameViewTexture->GetSRV(),
		viewportSize
		//ImVec2(0.f, .f), // uv start
		//ImVec2(0.55f, .55f), // uv end
		//ImVec4(1, 1, 1, 1) // tint color
		//ImVec4(1, 0, 0, 1) // border color
	);

	ImGui::GetWindowDrawList()->AddRect(tl, br, col);

	gs.gameScreenRect = rect + float4(
		windowRect.x,
		windowRect.y,
		windowRect.x,
		windowRect.y
	);
}
