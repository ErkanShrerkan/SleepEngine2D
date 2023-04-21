#include "pch.h"
#include "Transform.h"
#include "Entity.h"

Transform::Transform()
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
	myRotation += myRotation > 360.f ? -360.f : (myRotation < 0.f ? 360.f : 0);
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
	return GetTransform().GetUp().xy;
}

float2 Transform::GetRight()
{
	return GetTransform().GetRight().xy;
}

float4x4 Transform::GetTransform()
{
	float4x4 m = GetObjectSpaceTransform();
	if (GameObject().GetParentID() != UINT_MAX)
	{
		m = m * GameObject().GetParent().GetComponent<Transform>()->GetTransform();
	}
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
	return float4x4::CreateRotationAroundZ(Math::DegreeToRadian(-myRotation));
}

float4x4 Transform::GetTranslationMatrix()
{
	float4x4 t;
	t.SetRow(4, { myPosition, 0, 1 });
	return t;
}

float4x4 Transform::GetObjectSpaceTransform()
{
	float4x4 s = GetScaleMatrix();
	float4x4 r = GetRotationMatrix();
	float4x4 t = GetTranslationMatrix();
	return s * r * t;
}

void Transform::MoveObjectSpace(float2 aMovementVector)
{
	float4 rotatedMovementVector = GetRotationMatrix() * float4(aMovementVector, 0, 1);
	SetPosition(myPosition + rotatedMovementVector.xy);
}

void Transform::MoveWorldSpace(float2 aMovementVector)
{
	SetPosition(myPosition + aMovementVector);
}

void Transform::Start()
{
	Expose(myScale, "Scale", .1f);
	Expose(myPosition, "Position", .25f);
	Expose(myRotation, "Rotation", .1f, Expose::eBounds::Loop, float2(0.f, 360.f));
}
