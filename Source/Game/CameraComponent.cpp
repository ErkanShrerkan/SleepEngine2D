#include "pch.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent(
	float2 anAspectRatio,
	float anFOV,
	bool aIsPerspective,
	float aNear,
	float aFar)
	:
	myAspectRatio(anAspectRatio),
	myZoom(100),
	myIsPerspective(aIsPerspective),
	myNear(aNear),
	myFar(aFar),
	myFOV(anFOV)
{
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::Start()
{
	UpdateProjection();

	Expose(myAspectRatio, "Aspect Ratio", 1.f, Expose::eBounds::Clamp, float2(1, 100));
	Expose(myNear, "Near Plane", 0.1f, Expose::eBounds::Clamp, float2(0.0001f, 100'00.f));
	Expose(myFar, "Far Plane", 0.1f, Expose::eBounds::Clamp, float2(100'001.f, 100000'00.f));
	Expose(myFOV, "FOV", 0.1f, Expose::eBounds::Clamp, float2(1, 179));
	Expose(myZoom, "Zoom %", 1.f, Expose::eBounds::Clamp, float2(0, FLT_MAX));
	Expose(myIsPerspective, "Is Perspective");
}

void CameraComponent::UpdateProjection()
{
	if (myIsPerspective)
	{
		StartPerspective();
	}
	else
	{
		StartOrthographic();
	}
}

void CameraComponent::StartPerspective()
{
	float horizontalFoVinRadians = Math::DegreeToRadian(myFOV/* * myZoom * 0.01f*/);

	float verticalFoVinRadians = 2.f * std::atanf(std::tanf(horizontalFoVinRadians * 0.5f) * (myAspectRatio.y / myAspectRatio.x));

	float myXScale = 1.f / std::tanf(horizontalFoVinRadians * 0.5f);
	float myYScale = 1.f / std::tanf(verticalFoVinRadians * 0.5f);

	float planeConstant = myFar / (myFar - myNear);

	myProjection(1, 1) = myXScale;
	myProjection(2, 2) = myYScale;
	myProjection(3, 3) = planeConstant;
	myProjection(3, 4) = 1.f;
	myProjection(4, 3) = (-myFar * myNear) / (myFar - myNear);
	myProjection(4, 4) = 0.0f;
}

void CameraComponent::StartOrthographic()
{
	float zoom = myZoom * 0.01f;
	float f = 10.f;
	float n = 0.01f;
	float aspect = myAspectRatio.x / myAspectRatio.y;
	myProjection(1, 1) = 2.f / (aspect * zoom);
	myProjection(2, 2) = 2.f / zoom;
	myProjection(3, 3) = 1.f / (f - n);
	myProjection(4, 3) = n / (n - f);
	myProjection(4, 4) = 1.0f;
}
