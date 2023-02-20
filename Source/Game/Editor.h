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
		void OnImGui();

	private:
		GameManager myGM;
		bool myDisplay = true;
	};
}

