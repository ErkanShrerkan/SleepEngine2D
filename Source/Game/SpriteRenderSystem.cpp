#include "pch.h"
#include "SpriteRenderSystem.h"
#include "GameManager.h"
#include "Sprite.h"
#include "Entity.h"
#include "Transform.h"

void SpriteRenderSystem::Update()
{
	for (auto& [entity, component] : myGameManager->GetComponentMap<Sprite>().map)
	{
		//float2 pos = myGameManager->GetEntity(entity).GetComponent<Transform>().GetPosition();
		//component->SetPosition(pos);
		component->DrawRect();
		component->Render();
	}
}
