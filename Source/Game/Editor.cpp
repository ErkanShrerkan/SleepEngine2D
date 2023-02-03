#include "pch.h"
#include "Editor.h"
#include <Engine/Input.h>
#include <ImGui/imgui.h>

Game::Editor::~Editor()
{
}

bool Game::Editor::Init()
{
	myGM.Init();
	return true;
}

bool Game::Editor::Update()
{
	OnImGui();
	return myIsRunning;
}

void Game::Editor::RecieveMessage(eMessage aMessage)
{
	aMessage;
}

void Game::Editor::OnImGui()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	if (ImGui::Begin("Editor", 0, 
		ImGuiWindowFlags_NoScrollbar 
		| ImGuiWindowFlags_NoMove 
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::Button("Play"))
		{
			myIsRunning = false;
		}
	}
	ImGui::End();

	if (Input::GetInputDown(eInputEvent::O))
	{
		myIsRunning = false;
	}
}
