#pragma once
#include "Component.h"

class Rigidbody : public Component
{
public:
    Rigidbody();
    ~Rigidbody();

public:
    virtual void Start() override;
};

