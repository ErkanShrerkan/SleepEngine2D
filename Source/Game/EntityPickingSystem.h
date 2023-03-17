#pragma once
#include "System.h"

class EntityPickingSystem : public System
{
public:
	EntityPickingSystem(GameManager* aGM)
		: System::System(aGM)
	{

	}

	virtual void Update() override;
};
