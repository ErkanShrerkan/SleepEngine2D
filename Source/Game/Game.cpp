#include "pch.h"
#include "Game.h"
#include "Game\JsonManager.h"
#include <Engine\Engine.h>

#include <Engine\Camera.h>
#include <Engine\ContentLoader.h>
#include <Engine\EnvironmentLight.h>
#include <Engine/PointLight.h>
#include <Engine/Input.h>

#include <physx/PxPhysicsAPI.h>
#include <Engine/AudioEngine.h>
#include <Engine/LineDrawer.h>
#include <fstream>

#include "Expose.h"
#include "Postmaster.h"
#include <Game\Globals.h>
#include <Engine\DebugProfiler.h>

namespace Game
{
	Game::~Game()
	{
		Postmaster::GetInstance().UnSubscribe(this, eMessage::eUI_Element_Button_Exit);
		Postmaster::GetInstance().UnSubscribe(this, eMessage::eGame_Event_PauseGame);
		Postmaster::GetInstance().UnSubscribe(this, eMessage::eGame_Event_ResumeGame);
		Postmaster::GetInstance().UnSubscribe(this, eMessage::eQuitGame);
	}

	bool Game::Init()
	{
		Input::SetIsEditing(false);

		Postmaster::GetInstance().Subscribe(this, eMessage::eUI_Element_Button_Exit);
		Postmaster::GetInstance().Subscribe(this, eMessage::eGame_Event_PauseGame);
		Postmaster::GetInstance().Subscribe(this, eMessage::eGame_Event_ResumeGame);
		Postmaster::GetInstance().Subscribe(this, eMessage::eQuitGame);

		myGM.Init();

		SE::CEngine::GetInstance()->SetGameManagerRef(&myGM);

		return true;
	}

	bool printAddress = true;
	bool Game::Update()
	{
		if (!myIsPaused)
		{
			myGM.Update();
		}

		//EXPOSE()

		/*if (myPauseStateChanged)
		{
			myPauseStateChanged = false;
			myIsPaused = !myIsPaused;
			SE::CEngine::GetInstance()->GetActiveScene()->SetPaused(myIsPaused);
		}*/

		if (Input::GetInputReleased(eInputEvent::F4))
		{
			return false;
		}

		ImGui::Begin("Profiler##Game");
		Singleton<SE::Debug::CDebugProfiler>().Render();
		ImGui::End();

		return myIsRunning;
	}

	void Game::RecieveMessage(eMessage aMessage)
	{
		if (aMessage == eMessage::eUI_Element_Button_Exit)
		{
			myIsRunning = false;
		}
		switch (aMessage)
		{
		case eMessage::eGame_Event_ResumeGame:
		{
			if (myIsPaused)
			{
				myPauseStateChanged = true;
			}
			//myIsPaused = false;
			//SE::CEngine::GetInstance()->GetActiveScene()->SetPaused(false);
			break;
		}
		case eMessage::eGame_Event_PauseGame:
		{
			if (!myIsPaused)
			{
				myPauseStateChanged = true;
			}
			//myIsPaused = true;
			//SE::CEngine::GetInstance()->GetActiveScene()->SetPaused(true);
			break;
		}
		case eMessage::eQuitGame:
		{
			myIsRunning = false;
			break;
		}
		default:
			break;
		}
	}
}


