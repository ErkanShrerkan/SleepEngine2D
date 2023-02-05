#include "pch.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent(Entity*& anEntity)
	:Component::Component(anEntity)
{
}

CameraComponent::CameraComponent(float2 aDimension, Entity*& anEntity)
	:Component::Component(anEntity), myDimensions(aDimension)
{

}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::Start()
{
	float f = 10.f;
	float n = 0.1f;
	myProjection(1, 1) = 2.f / myDimensions.x;
	myProjection(2, 2) = 2.f / myDimensions.y;
	myProjection(3, 3) = 1.f / (f - n);
	myProjection(4, 3) = n / (n - f);
	myProjection(4, 4) = 1.0f;

	Expose(myDimensions, "Dimensions");
}

void CameraComponent::Update()
{
	myProjection(1, 1) = 2.f / myDimensions.x;
	myProjection(2, 2) = 2.f / myDimensions.y;
}
