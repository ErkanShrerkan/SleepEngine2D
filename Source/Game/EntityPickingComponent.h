#pragma once
#include "Component.h"

class EntityPickingSystem;

class EntityPickingComponent : public Component, public InputObserver
{
	friend class EntityPickingSystem;
public:
	~EntityPickingComponent();
	virtual void Start() override;
	uint GetPickedEntityID() { return myPickedEntityID; }
	void SetPickPos(float2 aPos) { myPickPos = aPos; }
	void SetPickedEntityID(uint anID) { myPickedEntityID = anID; }
	void Pick();

private:
	float2 myPickPos;
	uint myPickedEntityID = UINT_MAX;
	bool myShouldPick = false;
};

