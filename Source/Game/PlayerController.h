#pragma once
#include "Component.h"

class PlayerController : public Component, public InputObserver
{
public:
    PlayerController(Entity*& myEntity);
    ~PlayerController();

public:
    virtual void Update([[maybe_unused]] float aDeltaTime) override;
    virtual void Start() override;

private:
    void MoveRight();
    void MoveLeft();
    void MoveUp();
    void MoveDown();

private:
    float2 myPosition;
    float2 myMovement;
    float mySpeed = .5f;
};

