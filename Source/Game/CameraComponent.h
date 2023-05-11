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
    float4x4 GetProjection() { return myProjection; }
    void UpdateProjection();
    float GetAspectRatio() { return myAspectRatio.x / myAspectRatio.y; }
    float GetZoom() { return myZoom; }
    void Zoom(float aZoomMod) { myZoom *= aZoomMod; };
    void SetZoom(float aZoom) { myZoom = aZoom; };

private:
    float myZoom = 1000;
    float2 myAspectRatio = { 1, 1 };
    float4x4 myProjection;
};

