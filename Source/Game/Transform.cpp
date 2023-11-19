#include "pch.h"
#include "Transform.h"
#include "Entity.h"

Transform::Transform()
{
}

Transform::~Transform()
{
}

void Transform::Move(float3 aMovementVector, Space aSpace)
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

void Transform::Rotate(float3 aRotaionInDegrees)
{
	SetRotation(GetRotation() + aRotaionInDegrees);
}

void Transform::SetTransform(const float4x4& aMatrix)
{
	SetScale(aMatrix);
	SetRotation(aMatrix);
	SetPosition(aMatrix);
}

void Transform::SetScale(const float4x4& aMatrix)
{
	SetScale(
		{ 
			aMatrix.GetRight().Length(), 
			aMatrix.GetUp().Length(), 
			aMatrix.GetForward().Length() 
		});
}

void Transform::SetRotation(const float4x4& aMatrix)
{
	Quaternion q(aMatrix);
	float3 e = q.GetEuler();
	e = { RAD_TO_DEG(e.x), RAD_TO_DEG(e.y), RAD_TO_DEG(e.z) };
	SetRotation(e);
}

void Transform::SetPosition(const float4x4& aMatrix)
{
	SetPosition(aMatrix.GetPosition());
}

void Transform::SetPosition(float3 aPos)
{
	myTransform.myPosition = aPos;
}

void Transform::SetRotation(float3 aRotationInDegrees)
{
	myTransform.myRotation = aRotationInDegrees;
	InBoundsRotation();
}

void Transform::SetScale(float3 aScale)
{
	myTransform.myScale = aScale;
}

float3 Transform::GetRotation()
{
	return myTransform.myRotation;
}

float3 Transform::GetScale()
{
	return myTransform.myScale;
}

float3 Transform::GetPosition()
{
	return myTransform.myPosition;
}

float3 Transform::GetUp()
{
	return GetTransform().GetUp();
}

float3 Transform::GetRight()
{
	return GetTransform().GetRight();
}

float4x4 Transform::GetTransform()
{
	float4x4 m = GetObjectSpaceTransform();
	Entity& go = GameObject();
	if (!go.HasParent())
		return m;

	m *= GetParentWorldSpaceTransform();
	return m;
}

float4x4 Transform::GetScaleMatrix()
{
	float4x4 s;

	s(1, 1) = myTransform.myScale.x;
	s(2, 2) = myTransform.myScale.y;
	s(3, 3) = myTransform.myScale.z;

	return s;
}

float4x4 Transform::GetRotationMatrix()
{
	Quaternion q(myTransform.myRotation);
	float4x4 m;
	q.getRotationMatrix(m, true);
	return m;
}

float4x4 Transform::GetTranslationMatrix()
{
	float4x4 t;
	t.SetRow(4, { myTransform.myPosition, 1 });
	return t;
}

float4x4 Transform::GetObjectSpaceTransform()
{
	if (!HasChanged())
	{
		return myCachedMatrix;
	}

	memcpy(&myCachedTransform, &myTransform, sizeof(TransformData));

	float4x4 s = GetScaleMatrix();
	float4x4 r = GetRotationMatrix();
	float4x4 t = GetTranslationMatrix();

	myCachedMatrix = s * r * t;
	return myCachedMatrix;
}

float4x4 Transform::GetParentWorldSpaceTransform()
{
	Entity& go = GameObject();
	if (!go.HasParent())
		return float4x4();

	Transform& t = *go.GetParent().GetComponent<Transform>();
	return t.GetTransform();
}

void Transform::MoveObjectSpace(float3 aMovementVector)
{
	float4 rotatedMovementVector = GetRotationMatrix() * float4(aMovementVector, 1);
	SetPosition(myTransform.myPosition + rotatedMovementVector.xyz);
}

void Transform::MoveWorldSpace(float3 aMovementVector)
{
	SetPosition(myTransform.myPosition + aMovementVector);
}

void Transform::InBoundsRotation()
{
	InBoundsRotationAxis(myTransform.myRotation.x);
	InBoundsRotationAxis(myTransform.myRotation.y);
	InBoundsRotationAxis(myTransform.myRotation.z);
}

void Transform::InBoundsRotationAxis(float& anAxis)
{
	anAxis += anAxis > 360.f ? -360.f : (anAxis < 0.f ? 360.f : 0);
}

bool Transform::HasChanged()
{
	return memcmp(&myTransform, &myCachedTransform, sizeof(TransformData)) != 0;
}

void Transform::Start()
{
	Expose(myTransform.myScale, "Scale", .01f);
	Expose(myTransform.myPosition, "Position", .25f);
	Expose(myTransform.myRotation, "Rotation", .1f, Expose::ePickMode::Drag, Expose::eBounds::Loop, float2(0.f, 360.f));
}
