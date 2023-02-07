#pragma once
#include "Component.h"


class CameraComponent : public Component
{
public: 
    CameraComponent(){}
    CameraComponent(float2 anAspectRatio, float aZoom);
    ~CameraComponent();

public:
    virtual void Start() override;
    virtual void Update() override;
    float4x4 GetProjection() { return myProjection; }

private:
    float4x4 myProjection;
    float2 myAspectRatio = { 1, 1 };
    float myZoom = 1000;
};

