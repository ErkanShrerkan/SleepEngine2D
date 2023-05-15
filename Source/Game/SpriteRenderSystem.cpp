#include "pch.h"
#include "SpriteRenderSystem.h"
#include "GameManager.h"
#include "Sprite.h"
//#include "Entity.h"
//#include "Transform.h"

void SpriteRenderSystem::Update()
{
	for (auto& [entity, component] : myGameManager->GetComponentMap<Sprite>().map)
	{
		if (!myGameManager->IsEntityAndComponentActive(entity, component))
			continue;

		component.Render();
	}
}
