#pragma once
#include "Component.h"

class Transform;

class TestClass
{
    int nuthin;
};

class PlayerController : public Component, public InputObserver
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
    Transform* myTransformRef;
    float2 myMovement;
    float mySpeed = 500;
    bool myCanMove = false;
};

