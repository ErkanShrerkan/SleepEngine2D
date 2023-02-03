#pragma once
#include "System.h"

using PolygonCollider = std::vector<float2>;
class CollisionSystem : public System
{
public:
	CollisionSystem(GameManager* aGM)
		: System::System(aGM){}

	virtual void Update() override;

private:
	bool Intersects(const PolygonCollider& a, const PolygonCollider& b);
	float Min(float a, float b)
	{
		return (((a) < (b)) ? (a) : (b));
	}
	float Max(float a, float b)
	{
		return (((a) > (b)) ? (a) : (b));
	}
};


