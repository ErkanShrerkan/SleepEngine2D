#pragma once
#include "System.h"

class SpriteRenderSystem : public System
{
public:
	SpriteRenderSystem(GameManager* aGM)
		: System::System(aGM)
	{

	}

	virtual void Update() override;
};

