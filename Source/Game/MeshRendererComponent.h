#pragma once
#include "Component.h"

class MeshComponent;
class MaterialComponent;
class Transform;

class MeshRendererComponent : public Component
{
public:
	MeshRendererComponent();
	~MeshRendererComponent();

public:
	virtual void Start() override;

	void Render();

private:
	MeshComponent* myMeshRef;
	MaterialComponent* myMatRef;
	Transform* myTransformRef;
};
