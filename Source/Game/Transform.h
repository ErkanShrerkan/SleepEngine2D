#pragma once
#include "Component.h"

class Transform : public Component
{
public:
    enum class Space
    {
        World,
        Object,
    };

public:
    Transform();
    ~Transform();

    void SetPosition(float2 aPos);
    /// <summary>
    /// Moves the Transform relative to its position
    /// </summary>
    /// <param name="aMovementVector">: The direction and magnitude to translate by</param>
    /// <param name="aSpace">: The space in wich aMovementVecor exists</param>
    void Move(float2 aMovementVector, Space aSpace = Space::World);
    void Rotate(float aRotaionInDegrees);
    void SetRotation(float aRotationInDegrees);
    void SetScale(float2 aScale);
    float2 GetScale();
    float2 GetPosition();
    float2 GetUp();
    float2 GetRight();
    float4x4 GetTransform();
    float4x4 GetScaleMatrix();
    float4x4 GetRotationMatrix();
    float4x4 GetTranslationMatrix();
    float4x4 GetObjectSpaceTransform();

private:
    void MoveObjectSpace(float2 aMovementVector);
    void MoveWorldSpace(float2 aMovementVector);

public:
    virtual void Start() override;

private:
    float2 myScale = { 1, 1 };
    float2 myPosition;
    float myRotation = 0;
};

