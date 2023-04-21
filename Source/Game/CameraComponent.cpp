#include "pch.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent(float2 anAspectRatio, float aZoom)
	: myAspectRatio(anAspectRatio), myZoom(aZoom)
{

}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::Start()
{
	float f = 10.f;
	float n = 0.01f;
	float aspect = myAspectRatio.x / myAspectRatio.y;
	myProjection(1, 1) = 2.f / (aspect * myZoom);
	myProjection(2, 2) = 2.f / myZoom;
	myProjection(3, 3) = 1.f / (f - n);
	myProjection(4, 3) = n / (n - f);
	myProjection(4, 4) = 1.0f;

	Expose(myAspectRatio, "Aspect Ratio", 1.f, Expose::eBounds::Clamp, float2(1, 100));
	Expose(myZoom, "Zoom", 1.f, Expose::eBounds::Clamp, float2(0, FLT_MAX));
}

void CameraComponent::UpdateProjection()
{
	float aspect = myAspectRatio.x / myAspectRatio.y;
	myProjection(1, 1) = 2.f / (aspect * myZoom);
	myProjection(2, 2) = 2.f / myZoom;
}
