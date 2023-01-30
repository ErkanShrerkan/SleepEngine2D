#pragma once
#include "System.h"
#include "Sprite.h"

class SpriteRenderSystem : public System
{
public:
	SpriteRenderSystem(GameManager* aGM)
		: System::System(aGM)
	{

	}

	virtual void Update([[maybe_unused]] float aDeltaTime) override;
};

