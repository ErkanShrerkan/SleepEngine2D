#pragma once
#include "System.h"

class CollisionSystem : public System
{
public:
	CollisionSystem(GameManager* aGM)
		: System::System(aGM)
	{

	}

	virtual void Update([[maybe_unused]] float aDeltaTime) override;
};


