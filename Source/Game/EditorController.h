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
    void MoveRight();
    void MoveLeft();
    void MoveUp();
    void MoveDown();
    void ZoomOut();
    void ZoomIn();
    void ToggleShift(bool aBool) { myShiftDown = aBool; }
    void Pick();
    bool MouseIsOverGameWindow();

private:
    CameraComponent* myCam = nullptr;
    float2 myMovement;
    float mySpeed = 500;
    bool myCanMove = false;
    bool myShiftDown = false;
};

