#pragma once
#include "Observer.h"
#include "Observer.h"
#include "Process.h"
#include "GameManager.h"
#include <filesystem>
#include <d3d11.h>

namespace SE
{
	class CTexture;
}

class EntityPickingComponent;
namespace Game
{
	class Editor : public Observer, public Process
	{
	public:
		~Editor();
		virtual bool Init() override;
		virtual bool Update() override;
		void RecieveMessage(eMessage aMessage) override;

	private:
		// ImGui
		void OnImGui();
		void BlackScreen();
		void SceneHierarchy();
		void Inspector();
		void Profiler();
		void ContentBrowser();
		void Controls();
		void GameWindow();
		void DirectoryNavigator();
		void Assets();
		void EditorDockSpace();

		// Debug
		void DebugDrawRect();
		void DebugDrawMousePos();

		// Editor Functions
		bool ValidSelection();
		void AddEntityComponent();
		void AddEntity();
		void SelectEntity();
		void ModifyValues();
		void ListEntityRecursive(uint anID);
		void BuildHierarchy();
		void HandleSelection();
		void HandleHierarchySelection(uint anID, bool isHovered);
		void CheckClearThumbnails();
		void InternalUpdate();
		void DrawWorldGrid();

		void LoadThumbnail(const std::string& anImgPath);
		ID3D11ShaderResourceView* const GetThumbnail(const std::string& anImgPath) const noexcept;
	
	private:
		GameManager myGM;
		EntityPickingComponent* myPicker;
		bool myDisplay = true;

		// Editor control variables
		std::filesystem::path myCurrentPath;
		std::unordered_map<std::string, sptr(SE::CTexture)> myAssetThumbnails;
		std::unordered_map<uint, bool> myShowChildrenRecord;
		std::map<uint, std::set<uint>> myEntityHierarchy;
		uint mySelectedEntity = UINT_MAX;
		uint mySelectedEntityLastFrame = UINT_MAX;
		uint myInitiallySelectedEntity = UINT_MAX;
		uint myEditorEntityID;
		bool myEntityHierarchyNeedsUpdating = false;
		bool myHoversInitiallySelectedEntity = false;
		bool myClearThumbnails = false;
	};
}

