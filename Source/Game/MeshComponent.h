#pragma once
#include "Component.h"

namespace SE
{
	class Model;
}

class MeshComponent : public Component
{
public:
	MeshComponent();
	~MeshComponent();

public:
	virtual void Start() override;
	virtual void Reload() override;
	SE::Model* GetMesh();

private:
	ExposableString myModelPath;
	SE::Model* myModel;
};
