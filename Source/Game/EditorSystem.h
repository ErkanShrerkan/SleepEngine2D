#pragma once
#include "System.h"

class EditorSystem : public System
{
public:
	EditorSystem(GameManager* aGM)
		: System::System(aGM)
	{

	}

	virtual void Update() override;
};

