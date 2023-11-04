#pragma once
#include "Component.h"

class Transform : public Component
{
public:
	enum class Space : char
	{
		World,
		Object,
	};

public:
	Transform();
	~Transform();

	virtual void Start() override;

	/// <summary>
	/// Moves the Transform relative to its position
	/// </summary>
	/// <param name="aMovementVector">: The direction and magnitude to translate by</param>
	/// <param name="aSpace">: The space in wich aMovementVecor exists</param>
	void Move(float3 aMovementVector, Space aSpace = Space::World);
	void Rotate(float3 aRotaionInDegrees);
	void SetTransform(const float4x4& aMatrix);
	void SetScale(const float4x4& aMatrix);
	void SetRotation(const float4x4& aMatrix);
	void SetPosition(const float4x4& aMatrix);
	void SetRotation(float3 aRotationInDegrees);
	void SetPosition(float3 aPos);
	void SetScale(float3 aScale);
	float3 GetRotation();
	float3 GetScale();
	float3 GetPosition();
	float3 GetUp();
	float3 GetRight();
	float4x4 GetTransform();
	float4x4 GetScaleMatrix();
	float4x4 GetRotationMatrix();
	float4x4 GetTranslationMatrix();
	float4x4 GetObjectSpaceTransform();
	float4x4 GetParentWorldSpaceTransform();

private:
	struct TransformData
	{
		float3 myScale = { 1.f, 1.f, 1.f };
		float3 myPosition = { 0.f, 0.f, 0.f };
		float3 myRotation = { 0.f, 0.f, 0.f };
	};

	void MoveObjectSpace(float3 aMovementVector);
	void MoveWorldSpace(float3 aMovementVector);
	void InBoundsRotation();
	void InBoundsRotationAxis(float& anAxis);
	bool HasChanged();
private:
	TransformData myTransform;
	TransformData myCachedTransform;
	float4x4 myCachedMatrix;
};

