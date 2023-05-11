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
    bool myCanMove = false;
    bool myShiftDown = false;
    float mySpeed = 500;
    float myShiftMult = 2;
    float2 myMovement;
    CameraComponent* myCam = nullptr;
};

