#include "pch.h"
#include "MaterialComponent.h"
#include <Engine\MaterialFactory.h>

MaterialComponent::MaterialComponent()
{
}

MaterialComponent::~MaterialComponent()
{
}

void MaterialComponent::Start()
{
	myTextures.resize(8);

	Expose(myPS, "Pixel Shader");
	Expose(myVS, "Vertex Shader");

	int i = 1;
	for (auto& texture : myTextures)
	{
		std::string name = "Texture " + std::to_string(i);
		Expose(texture, name);
		i++;
	}
}

void MaterialComponent::Reload()
{
	myMaterial.Reset();
	myMaterial.SetPS(myPS.GetString());
	myMaterial.SetVS(myVS.GetString());

	for (auto& texture : myTextures)
	{
		myMaterial.AddTexture(texture.GetString());
	}
}

SE::Material MaterialComponent::GetMaterial()
{
	return myMaterial;
}
