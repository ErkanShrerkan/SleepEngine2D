#include "pch.h"
#include "MeshComponent.h"
#include <Engine\ModelFactory.h>

MeshComponent::MeshComponent()
{
}

MeshComponent::~MeshComponent()
{
}

void MeshComponent::Start()
{
	Expose(myModelPath, "Mesh Path");
}

void MeshComponent::Reload()
{
	myModel = Singleton<SE::ModelFactory>().GetModel(myModelPath.GetString());
}

SE::Model* MeshComponent::GetMesh()
{
	return myModel;
}
