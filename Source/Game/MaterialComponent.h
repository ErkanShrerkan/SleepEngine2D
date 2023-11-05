#pragma once
#include "Component.h"
#include <Engine\Material.h>
#include <Engine\Texture.h>

class MaterialComponent : public Component
{
public:
	MaterialComponent();
	~MaterialComponent();

public:
	virtual void Start() override;
	virtual void Reload() override;

	SE::Material GetMaterial();

private:
	ExposableString myPS;
	ExposableString myVS;
	std::vector<ExposableString> myTextures;
	SE::Material myMaterial;
};
