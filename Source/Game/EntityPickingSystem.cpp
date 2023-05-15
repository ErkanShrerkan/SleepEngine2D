#include "pch.h"
#include "GameManager.h"
#include "Entity.h"
#include "EntityPickingSystem.h"
#include "EntityPickingComponent.h"
#include "CameraComponent.h"
#include "Transform.h"
#include "Globals.h"
#include <Engine\LineDrawer.h>

void EntityPickingSystem::Update()
{
	auto& map = myGameManager->GetComponentMap<EntityPickingComponent>().map;

	CameraComponent* cam = nullptr;
	Entity* pickerEntity = nullptr;
	auto& gs = Singleton<GlobalSettings>();

	for (auto& [entityID, picker] : map)
	{
		if (!myGameManager->IsEntityAndComponentActive(entityID, picker))
			continue;

		pickerEntity = &myGameManager->GetEntity(entityID);
		cam = pickerEntity->GetComponent<CameraComponent>();

		if (!cam || !picker.myShouldPick)
			continue;

		picker.myShouldPick = false;

		float2 pickPos = picker.myPickPos;
		float4 gameRect = gs.GetGameScreenNormalised();

		pickPos -= gameRect.xy;
		gameRect.zw -= gameRect.xy;
		gameRect.xy = { 0, 0 };

		pickPos.x /= gameRect.z;
		pickPos.y /= gameRect.w;
		pickPos.y = 1.f - pickPos.y;
		pickPos.x -= .5f;
		pickPos.y -= .5f;
		pickPos *= 2.f;

		if (pickPos.x < -1.f ||
			pickPos.x > 1.f ||
			pickPos.y < -1.f ||
			pickPos.y > 1.f)
		{
			return;
		}

		float ratio = cam->GetAspectRatio();
		float zoom = cam->GetZoom();

		pickPos.x *= zoom * ratio * .5f;
		pickPos.y *= zoom * .5f;

		pickPos += pickerEntity->GetComponent<Transform>()->GetTransform().GetPosition().xy;

		float worldDist = zoom / 20.f;
		float transformDistThresh = worldDist;
		transformDistThresh *= transformDistThresh; // LengthSqr

		Debug::DrawCircle(pickPos, worldDist / 10, true);

		bool picked = false;
		for (auto& [id, entity] : myGameManager->GetEntities())
		{
			float2 ePos = entity->GetComponent<Transform>()->GetTransform().GetPosition().xy;

			if (pickerEntity->GetID() == id || (ePos - pickPos).LengthSqr() > zoom * zoom)
				continue;

			Debug::DrawCircle(ePos, worldDist, true);
			if ((ePos - pickPos).LengthSqr() < transformDistThresh)
			{
				picker.SetPickedEntityID(id);
				picked = true;
				//printf("Entity Picked: %i\n", id);
				break;
			}
		}

		if (!picked)
			picker.SetPickedEntityID(NULL_ENTITY);
	}
}
