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
	myPosition = aPos;
}

void Transform::Move(float2 aMovementVector, Space aSpace)
{
	switch (aSpace)
	{
	case Space::World:
		MoveWorldSpace(aMovementVector);
		break;
	case Space::Object:
		MoveObjectSpace(aMovementVector);
		break;
	default:
		break;
	}
}

void Transform::Rotate(float aRotaionInDegrees)
{
	SetRotation(myRotation + aRotaionInDegrees);
}

void Transform::SetRotation(float aRotationInDegrees)
{
	myRotation = aRotationInDegrees;
}

void Transform::SetScale(float2 aScale)
{
	myScale = aScale;
}

float2 Transform::GetScale()
{
	return myScale;
}

float2 Transform::GetPosition()
{
	return myPosition;
}

float2 Transform::GetUp()
{
	float4x4 m = GetTransform();
	return m.GetUp().xy;
}

float2 Transform::GetRight()
{
	float4x4 m = GetTransform();
	return m.GetRight().xy;
}

float4x4 Transform::GetTransform()
{
	// TODO: transform from parent space to world space 
	float4x4 m;
	float4x4 s = GetScaleMatrix();
	float4x4 r = GetRotationMatrix();
	float4x4 t = GetTranslationMatrix();

	m = s * r * t;

	return m;
}

float4x4 Transform::GetScaleMatrix()
{
	float4x4 s;

	s(1, 1) = myScale.x;
	s(2, 2) = myScale.y;

	return s;
}

float4x4 Transform::GetRotationMatrix()
{
	return float4x4::CreateRotationAroundZ(Math::DegreeToRadian(myRotation));
}

float4x4 Transform::GetTranslationMatrix()
{
	float4x4 t;
	t.SetRow(4, { myPosition, 0, 1 });
	return t;
}

void Transform::MoveObjectSpace(float2 aMovementVector)
{
	float4x4 r = GetRotationMatrix();
	auto rmv = r * float4(aMovementVector, 0, 1);
	SetPosition(myPosition + rmv.xy);
}

void Transform::MoveWorldSpace(float2 aMovementVector)
{
	SetPosition(myPosition + aMovementVector);
}

void Transform::Start()
{
	Expose(myPosition, "Position");
	Expose(myRotation, "Rotation");
	Expose(myScale, "Scale");
}

void Transform::Update()
{
}
