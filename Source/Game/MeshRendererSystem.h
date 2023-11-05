#pragma once
#include "System.h"

class MeshRendererSystem : public System
{
public:
	MeshRendererSystem(GameManager* aGM)
		: System::System(aGM)
	{

	}

	virtual void Update() override;
};
