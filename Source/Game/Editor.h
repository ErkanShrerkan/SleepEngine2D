#pragma once
#include "Observer.h"
#include "Observer.h"
#include "Process.h"
#include "GameManager.h"

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
		void SceneHierarchy();
		void Inspector();
		void Profiler();
		void ContentBrowser();
		void Controls();

		// Editor Functions
		void AddEntityComponent();
		void AddEntity();
		void SelectEntity();
		void ModifyValues();
		void ListEntityRecursive(uint anID);
		bool ValidSelection();
		void BuildHierarchy();

	private:
		GameManager myGM;
		bool myDisplay = true;

		// Editor control variables
		uint mySelectedEntity = UINT_MAX;
		std::unordered_map<uint, bool> myShowChildrenRecord;
		std::map<uint, std::set<uint>> myEntityHierarchy;
		bool myEntityHierarchyNeedsUpdating = false;
	};
}

