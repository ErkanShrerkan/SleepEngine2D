#pragma once
#include "Component.h"


class CameraComponent : public Component
{
public: 
    CameraComponent(Entity*& anEntity);
    CameraComponent(float2 aDimension, Entity*& anEntity);
    ~CameraComponent();

public:
    virtual void Start() override;
    virtual void Update() override;
    float4x4 GetProjection() { return myProjection; }

private:
    float2 myDimensions = { 16, 9 };
    float4x4 myProjection;
};

