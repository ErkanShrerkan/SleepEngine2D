#pragma once
#include "Component.h"

class PlayerController : public Component
{
public:
    PlayerController(Entity*& myEntity);
    ~PlayerController();

public:
    virtual void Update([[maybe_unused]] float aDeltaTime) override;
    virtual void Start() override;

private:
    float2 myPosition;
    float2 myMovement;
    float mySpeed = .5f;
};

