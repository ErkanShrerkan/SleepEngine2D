#pragma once
#include "Component.h"

class Transform : public Component
{
public:
    Transform(Entity*& myEntity);
    ~Transform();

    float3x3 myTransform;

public:
    virtual void Start() override;
};

