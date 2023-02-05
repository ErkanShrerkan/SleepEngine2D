#pragma once
#include "Component.h"

class Transform : public Component
{
public:
    Transform(Entity*& myEntity);
    ~Transform();

    void SetPosition(float2 aPos);
    void Move(float2 aMovementVector);
    void Rotate(float aRotaionInDegrees);
    void SetRotation(float aRotationInDegrees);
    float2 GetPosition();
    float2 GetUp();
    float2 GetRight();
    float4x4 GetMatrix() { return myTransform; }

public:
    virtual void Start() override;
    virtual void Update() override;

private:
    float4x4 myTransform;
    float2 myPosition;
    float myRotation = 0;
};

