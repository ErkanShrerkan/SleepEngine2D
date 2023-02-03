#pragma once
#include <vector>
#include "Observer.h"
#include "Process.h"
#include "GameManager.h"

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
		bool myPauseStateChanged = false;
		float piTime = 0;
	};
}