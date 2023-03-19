#pragma once
#include "Component.h"

class Collider : public Component
{
public:
    enum class eType
    {
        Circle,
        Quad,
    };
    Collider(){}
    Collider(eType aType);
    ~Collider();

    bool isRigidBody = false;

    float2 pos;
    float radius = 0;

public:
    virtual void Start() override;
};

