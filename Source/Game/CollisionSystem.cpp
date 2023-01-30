#include "pch.h"
#include "CollisionSystem.h"
#include "GameManager.h"
#include "Collider.h"
#include <Engine\LineDrawer.h>
#include "Entity.h"
#include "Transform.h"

void CollisionSystem::Update(float aDeltaTime)
{
	aDeltaTime;
	auto& colliders = myGameManager->GetComponentMap<Collider>();

	// upgrade with spacial partitioning
	for (auto& [entity0, component0] : colliders.map)
	{
		//for (auto& [entity1, component1] : colliders.map)
		//{
		//	if (entity0 != entity1)
		//	{
		//		// solve collision
		//	}
		//}

		// TODO: fix 3x3 methods to orient collider in object space
		float2 objectPos = myGameManager->GetEntity(entity0).GetComponent<Transform>().myTransform.GetPosition();
		objectPos += component0->pos;

		float2 firstPoint = { 0, 0 };
		float2 lastPoint = { 0, 0 };
		float r = component0->radius;
		bool first = true;
		for (float a = 0; a <= 2 * 3.1415927f; a += /*2 * */.31415927f)
		{
			float x = (r * cos(a)) * (9.f / 16);
			float y = r * sin(a);

			if (first)
			{
				first = false;
				firstPoint = { x, y };
				lastPoint = firstPoint;
			}
			else
			{
				Debug::DrawLine2D({ objectPos.x + lastPoint.x, objectPos.y + lastPoint.y }, { x + objectPos.x, objectPos.y + y /** (16.f / 9)*/ }, { 1, 1, 1, 1 });
				lastPoint = { x, y };
			}
		}
		Debug::DrawLine2D({ objectPos.x + lastPoint.x, objectPos.y + lastPoint.y }, { firstPoint.x + objectPos.x, objectPos.y + firstPoint.y }, { 1, 1, 1, 1 });
	}
}
