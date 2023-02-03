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
	ImGui::SetNextWindowSize(ImVec2(500, 800), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	if (ImGui::Begin("Editor", 0,
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		//ImGuiWindowFlags_AlwaysAutoResize
		ImGuiWindowFlags_None
	))
	{
		if (ImGui::Button("Play"))
		{
			myIsRunning = false;
		}
	}
	myGM.OnImGui();
	ImGui::End();

	if (Input::GetInputDown(eInputEvent::O))
	{
		myIsRunning = false;
	}
}
