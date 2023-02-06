#include "pch.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent(Entity*& anEntity)
	:Component::Component(anEntity)
{
}

CameraComponent::CameraComponent(float2 anAspectRatio, Entity*& anEntity)
	:Component::Component(anEntity), myAspectRatio(anAspectRatio)
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

	Expose(myAspectRatio, "Aspect Ratio");
	Expose(myZoom, "Zoom");
}

void CameraComponent::Update()
{
	float aspect = myAspectRatio.x / myAspectRatio.y;
	myProjection(1, 1) = 2.f / (aspect * myZoom);
	myProjection(2, 2) = 2.f / myZoom;
}
