#include "pch.h"
#include "MeshRendererSystem.h"
#include "GameManager.h"
#include "MeshRendererComponent.h"
#include "AnimatorComponent.h"

void MeshRendererSystem::Update()
{
	GameManager& gm = *myGameManager;
	auto& meshRenderers = gm.GetComponentMap<MeshRendererComponent>().map;
	auto& animators = gm.GetComponentMap<AnimatorComponent>().map;

	for (auto& [entity, animator] : animators)
	{
		if (!gm.IsEntityAndComponentActive(entity, animator))
			continue;

		animator.Update();
	}

	for (auto& [entity, meshRenderer] : meshRenderers)
	{
		if (!gm.IsEntityAndComponentActive(entity, meshRenderer))
			continue;

		meshRenderer.Render();
	}
}
