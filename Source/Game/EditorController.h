#pragma once
#include "Component.h"

class EditorSystem;
class CameraComponent;

class EditorController : public Component, public InputObserver
{
	friend class EditorSystem;
public:
    ~EditorController();
	virtual void Start() override;

private:
    void Move();
    void Look();
    void SetCanLook();
    void MoveInput(float3 aMovement);
    void MoveY(int aDirection);
    void Zoom(float aZoomValue);
    void ToggleShift(bool aBool) { myShiftDown = aBool; }
    void Pick();
    bool MouseIsOverGameWindow();

private:
    bool myCanMove = false;
    bool myShiftDown = false;
    bool myCanLook = false;
    float mySpeed = 500;
    float myShiftMult = 2;
    float myCameraSensitivity = 0.15f;
    float3 myMovement;
    float2 myRotation = { 0, 0 };
    CameraComponent* myCam = nullptr;
};

