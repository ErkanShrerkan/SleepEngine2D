#pragma once
#include <vector>
#include "Observer.h"
#include "GameManager.h"
#include "Process.h"

namespace Game
{
	class Game : public Observer, public Process
	{
	public:
		~Game();
		virtual bool Init() override;
		virtual bool Update() override;
		void RecieveMessage(eMessage aMessage) override;

	private:

		GameManager myGM;

		bool myIsPaused = false;
		bool myIsRunning = true;
		bool myPauseStateChanged = false;
		float piTime = 0;
	};
}