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
	if (map.empty())
	{
		return;
	}

	CameraComponent* cam = nullptr;
	EntityPickingComponent* picker = nullptr;
	Entity* pickerEntity = nullptr;

	for (auto& [entityID, component] : map)
	{
		pickerEntity = &myGameManager->GetEntity(entityID);
		picker = component;
		cam = pickerEntity->GetComponent<CameraComponent>();
		if (cam)
		{
			break;
		}
	}

	if (!picker->myShouldPick)
	{
		return;
	}

	picker->myShouldPick = false;

	float2 pickPos = picker->myPickPos;
	float4 gameRect = Singleton<GlobalSettings>().gameWindowRect;

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

	pickPos.x *= zoom * ratio / 2;
	pickPos.y *= zoom / 2;

	pickPos += pickerEntity->GetComponent<Transform>()->GetPosition();

	float worldDist = zoom / 20.f;
	float transformDistThresh = worldDist;
	transformDistThresh *= transformDistThresh; // LengthSqr

	Debug::DrawCircle(pickPos, worldDist / 10, true);

	for (auto& [id, entity] : myGameManager->GetEntities())
	{
		float2 ep = entity->GetComponent<Transform>()->GetPosition();
		if (pickerEntity->GetID() == id || (ep - pickPos).LengthSqr() > zoom * zoom)
		{
			continue;
		}

		Debug::DrawCircle(ep, worldDist, true);
		if ((ep - pickPos).LengthSqr() < transformDistThresh)
		{
			picker->SetPickedEntityID(id);
			printf("Entity Picked: %i\n", id);
			return;
		}
	}

	picker->SetPickedEntityID(UINT_MAX);
}
