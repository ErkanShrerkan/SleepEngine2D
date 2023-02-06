#pragma once
#include "Component.h"


class CameraComponent : public Component
{
public: 
    CameraComponent(Entity*& anEntity);
    CameraComponent(float2 anAspectRatio, Entity*& anEntity);
    ~CameraComponent();

public:
    virtual void Start() override;
    virtual void Update() override;
    float4x4 GetProjection() { return myProjection; }

private:
    float4x4 myProjection;
    float2 myAspectRatio = { 16, 9 };
    float myZoom = 10;
};

