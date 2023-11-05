#include "pch.h"
#include "MeshRendererSystem.h"
#include "GameManager.h"
#include "MeshRendererComponent.h"

void MeshRendererSystem::Update()
{
	auto& mr = myGameManager->GetComponentMap<MeshRendererComponent>();

	for (auto& [entity, component] : mr.map)
	{
		if (!myGameManager->IsEntityAndComponentActive(entity, component))
			continue;

		component.Render();
	}
}
