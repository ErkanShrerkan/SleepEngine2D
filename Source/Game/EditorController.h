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
    void MoveInput(float2 aMovement);
    void Zoom(float aZoomValue);
    void ToggleShift(bool aBool) { myShiftDown = aBool; }
    void Pick();
    bool MouseIsOverGameWindow();

private:
    CameraComponent* myCam = nullptr;
    float2 myMovement;
    float mySpeed = 500;
    float myShiftMult = 2;
    bool myCanMove = false;
    bool myShiftDown = false;
};

