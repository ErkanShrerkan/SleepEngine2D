#pragma once
#include "Component.h"

class PlayerController : public Component, public InputObserver
{
public:
    PlayerController(Entity*& anEntity);
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
    //float2 myPosition;
    float2 myMovement;
    float mySpeed = .5f;
    bool myCanMove = false;
};

