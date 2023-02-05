#include "pch.h"
#include "Transform.h"

Transform::Transform(Entity*& myEntity)
	: Component::Component(myEntity)
{
}

Transform::~Transform()
{
}

void Transform::SetPosition(float2 aPos)
{
	myTransform.SetRow(4, { aPos, 0, 1 });
	myPosition = myTransform.GetPosition().xy;
}

void Transform::Move(float2 aMovementVector)
{
	float2 pos = GetPosition();
	SetPosition(pos + aMovementVector);
}

void Transform::Rotate(float aRotaionInDegrees)
{
	SetRotation(myRotation + aRotaionInDegrees);
}

void Transform::SetRotation(float aRotationInDegrees)
{
	myRotation = aRotationInDegrees;
	myTransform.CreateRotationAroundZ(Math::DegreeToRadian(aRotationInDegrees));
}

float2 Transform::GetPosition()
{
	return myTransform.GetPosition().xy;
}

float2 Transform::GetUp()
{
	return myTransform.GetUp().xy;
}

float2 Transform::GetRight()
{
	return myTransform.GetRight().xy;
}

void Transform::Start()
{
	Expose(myPosition, "Position");
	Expose(myRotation, "Rotation");
}

void Transform::Update()
{
	SetPosition(myPosition);
	SetRotation(myRotation);
}
