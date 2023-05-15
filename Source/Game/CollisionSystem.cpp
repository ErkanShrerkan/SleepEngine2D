#include "pch.h"
#include "CollisionSystem.h"
#include "GameManager.h"
#include "Collider.h"
#include <Engine\LineDrawer.h>
#include "Entity.h"
#include "Transform.h"

#include <vector>
#include <limits>

bool CollisionSystem::Intersects(const PolygonCollider& a, const PolygonCollider& b)
{
	float minOverlap = FLT_MAX;
	int minIndex = 0;

	for (int i = 0; i < b.size(); i++)
	{
		int j = (i + 1) % b.size();
		float2 edge = b[j] - b[i];
		float2 axis = { -edge.y, edge.x };
		axis.Normalize();

		float minA = 0, minB = 0, maxA = 0, maxB = 0;
		for (int k = 0; k < a.size(); k++)
		{
			float dot = axis.Dot(a[k] - a[0]);
			minA = Min(minA, dot);
			maxA = Max(maxA, dot);
		}
		for (int k = 0; k < b.size(); k++)
		{
			float dot = axis.Dot(b[k] - b[0]);
			minB = Min(minB, dot);
			maxB = Max(maxB, dot);
		}

		float overlap = Min(maxA, maxB) - Max(minA, minB);
		if (overlap < 0)
		{
			return false;
		}
		else if (overlap < minOverlap)
		{
			minOverlap = overlap;
			minIndex = i;
		}
	}

	//overlap
	float2 edge = b[(minIndex + 1) % b.size()] - b[minIndex];
	float2 axis = { -edge.y, edge.x };
	axis.Normalize();
	if (axis.Dot(a[0] - b[minIndex]) < 0)
	{
		axis = { -axis.x, -axis.y };
	}
	float2 MTV = axis * minOverlap;
	MTV;
	return true;
}

void CollisionSystem::Update()
{
	auto& colliders = myGameManager->GetComponentMap<Collider>();

	// upgrade with spacial partitioning
	for (auto& [entity0, component0] : colliders.map)
	{
		if (!myGameManager->IsEntityAndComponentActive(entity0, component0))
			continue;

		//for (auto& [entity1, component1] : colliders.map)
		//{
		//	if (entity0 != entity1)
		//	{
		//		// solve collision
		//	}
		//}

		float2 objectPos = myGameManager->GetEntity(entity0).GetComponent<Transform>()->GetPosition();
		objectPos += component0.pos;

		float2 firstPoint = { 0, 0 };
		float2 lastPoint = { 0, 0 };
		float r = component0.radius;
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
