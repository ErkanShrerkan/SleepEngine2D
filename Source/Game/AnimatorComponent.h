#pragma once
#include "Component.h"

class AnimatorComponent : public Component
{
public:
	AnimatorComponent();
	~AnimatorComponent();

public:
	virtual void Start() override;
	virtual void Update() override;
};
