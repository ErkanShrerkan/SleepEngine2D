#pragma once
#include "Component.h"

class EditorSystem;
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

private:
    float2 myMovement;
    float mySpeed = 500;
    bool myCanMove = false;
};

