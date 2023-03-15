#include "pch.h"
#include "Editor.h"
#include <Engine/Input.h>
#include <ImGui/imgui.h>
#include <Engine\Engine.h>

Game::Editor::~Editor()
{
}

bool Game::Editor::Init()
{
	myGM.Init();
	SE::CEngine::GetInstance()->SetGameManagerRef(&myGM);
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
	if (Input::GetInputReleased(eInputEvent::F4))
	{
		myIsRunning = false;
	}

	ImGui::DockSpaceOverViewport();
	ImGui::PushStyleColor(ImGuiCol_Button, { .1f, .1f, .1f, 1 });
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

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

	if (myDisplay)
	{
		myGM.OnImGui();
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}
