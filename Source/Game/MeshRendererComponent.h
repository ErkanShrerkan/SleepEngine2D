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

private:
	ExposableString myModelPath;
	ExposableString myMaterialPath;

	Transform* myTransformRef = nullptr;

	SE::Model* myModel;
	sptr(SE::Material) myMaterial = nullptr;
};
