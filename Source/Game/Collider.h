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

    float radius = 0;
    float2 pos;

public:
    virtual void Start() override;
};

