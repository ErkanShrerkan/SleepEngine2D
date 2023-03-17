#pragma once
#include "System.h"

class CameraSystem : public System
{
public:
	CameraSystem(GameManager* aGM)
		: System::System(aGM)
	{

	}

	virtual void Update() override;
};

