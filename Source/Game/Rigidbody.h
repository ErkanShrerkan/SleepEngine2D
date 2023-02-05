#pragma once
#include "Component.h"

class Rigidbody : public Component
{
public:
    Rigidbody(Entity*& anEntity);
    ~Rigidbody();

public:
    virtual void Update() override;
    virtual void Start() override;
};

