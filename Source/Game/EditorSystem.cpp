#include "pch.h"
#include "EditorSystem.h"
#include "EditorController.h"
#include "Entity.h"
#include "GameManager.h"
#include "EntityPickingComponent.h"
#include "Transform.h"
#include "CameraComponent.h"

void EditorSystem::Update()
{
	for (auto& [entity, component] : myGameManager->GetComponentMap<EditorController>().map)
	{
		if (!myGameManager->IsEntityAndComponentActive(entity, component))
			continue;

		component.Look();
		component.Move();

		Entity& object = myGameManager->GetEntity(entity);
		auto picker = object.GetComponent<EntityPickingComponent>();
		float2 mPos = Input::GetMousePos();
		picker->SetPickPos(mPos);
	}
}
