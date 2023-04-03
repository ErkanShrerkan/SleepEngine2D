#include "pch.h"
#include "CameraSystem.h"
#include "GameManager.h"
#include "CameraComponent.h"
//#include "Entity.h"

void CameraSystem::Update()
{
	for (auto& [entity, component] : myGameManager->GetComponentMap<CameraComponent>().map)
	{
		component.UpdateProjection();
	}
}
