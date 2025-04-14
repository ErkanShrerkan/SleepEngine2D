#pragma once
#include "Component.h"

class Transform;

class PlayerController : public Component
{
public:
    PlayerController();
    ~PlayerController();

public:
    virtual void Update() override;
    virtual void Start() override;

private:
    void MoveRight();
    void MoveLeft();
    void MoveUp();
    void MoveDown();
    void ToggleMovement();

private:
    bool myCanMove = false;
    float mySpeed = 500;
    float2 myMovement;
    Transform* myTransformRef;

    InputObserver myInputObserver;
};

