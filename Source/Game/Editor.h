#pragma once
#include "Observer.h"
#include "Process.h"
#include "GameManager.h"
#include "DynamicStringBuffer.h"
#include <Engine\Input.h>
#include <filesystem>
#include <d3d11.h>

namespace SE
{
	class CTexture;
}

class EntityPickingComponent;
namespace Game
{
	class Editor : 
		public Observer, 
		public Process, 
		public InputObserver
	{
	public:
		~Editor();
		virtual bool Init() override;
		virtual bool Update() override;
		void RecieveMessage(eMessage aMessage) override;

	private:
		// Not using ImGuizmo enums to minimize dependency 
		enum class eTransformOperation : char
		{
			None,
			Scale,
			Rotate,
			Translate,
		};

		enum class eTransformSpace : char
		{
			Local,
			World,
		};

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
		void RenderViewport();
		void RenderGizmos();
		float2 CalculateGameWindowRect();
		void RenderGameTextureToRect(float2 aViewPortSize);

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
		void InvalidateSelectionIfInvalid();
		void SetTransformOperation(eTransformOperation anOperation);
		void SetTransformSpace(eTransformSpace aSpace);

		void LoadThumbnail(const std::string& anImgPath);
		ID3D11ShaderResourceView* const GetThumbnail(const std::string& anImgPath) const noexcept;

	private:
		// Editor control variables
		eTransformOperation myOperation = eTransformOperation::Translate;
		eTransformSpace mySpace = eTransformSpace::World;
		bool myEntityHierarchyNeedsUpdating = false;
		bool myHoversInitiallySelectedEntity = false;
		bool myClearThumbnails = false;
		bool myIsTransforming = false;
		bool myDisplay = true;
		uint mySelectedEntity = NULL_ENTITY;
		uint mySelectedEntityLastFrame = NULL_ENTITY;
		uint myInitiallySelectedEntity = NULL_ENTITY;
		uint myEditorEntityID;
		EntityPickingComponent* myPicker;
		std::map<uint, std::set<uint>> myEntityHierarchy;
		DynamicStringBuffer mySceneLabel;
		std::filesystem::path myCurrentPath;
		std::unordered_map<uint, bool> myShowChildrenRecord;
		std::unordered_map<std::string, sptr(SE::CTexture)> myAssetThumbnails;
		GameManager myGM;
	};
}

