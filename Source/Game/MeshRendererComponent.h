#pragma once
#include "Component.h"

namespace SE
{
	class Model;
	class Material;
}

class Transform;

class MeshRendererComponent : public Component
{
public:
	MeshRendererComponent();
	~MeshRendererComponent();

public:
	virtual void Start() override;
	virtual void Reload() override;
	void Render();

private:
	ExposableString myModelPath;
	ExposableString myMaterialPath;

	Transform* myTransformRef;

	SE::Model* myModel = nullptr;
	SE::Material* myMaterial = nullptr;
};
