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
    void Move(float2 aMovementVector, Space aSpace = Space::World);
    void Rotate(float aRotaionInDegrees);
    void SetTransform(const float4x4& aMatrix);
    void SetScale(const float4x4& aMatrix);
    void SetRotation(const float4x4& aMatrix);
    void SetPosition(const float4x4& aMatrix);
    void SetRotation(float aRotationInDegrees);
    void SetPosition(float2 aPos);
    void SetScale(float2 aScale);
    float GetRotation();
    float2 GetScale();
    float2 GetPosition();
    float2 GetUp();
    float2 GetRight();
    float4x4 GetTransform();
    float4x4 GetScaleMatrix();
    float4x4 GetRotationMatrix();
    float4x4 GetTranslationMatrix();
    float4x4 GetObjectSpaceTransform();
    float4x4 GetParentWorldSpaceTransform();

private:
    void MoveObjectSpace(float2 aMovementVector);
    void MoveWorldSpace(float2 aMovementVector);
    void InBoundsRotation();

private:
    float2 myScale = { 1.f, 1.f };
    float2 myPosition = { 0.f, 0.f };
    float myRotation = 0.f;
};

