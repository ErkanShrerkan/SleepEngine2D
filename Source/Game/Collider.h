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
    Collider(eType aType, Entity*& myEntity);
    ~Collider();

    bool isRigidBody = false;

    float2 pos;
    float radius = 0;

public:
    virtual void Update() override;
    virtual void Start() override;
};

