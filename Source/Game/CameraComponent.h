#pragma once
#include "Component.h"

class CameraComponent : public Component
{
public: 
    CameraComponent(){}
    CameraComponent(float2 anAspectRatio, float anFOV = 90, bool aIsPerspective = true, float aNear = 1.f, float aFar = 1000'00.f);
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
    void StartPerspective();
    void StartOrthographic();

private:
    bool myIsPerspective = true;
    float myNear;
    float myFar;
    float myFOV;
    float myZoom = 100.f;
    float2 myAspectRatio = { 1, 1 };
    float4x4 myProjection;
};

