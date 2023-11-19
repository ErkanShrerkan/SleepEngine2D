#include "pch.h"
#include "MeshRendererComponent.h"

#include "Transform.h"
#include "Entity.h"

#include <Engine\RenderCommand.h>
#include <Engine\RenderCommandManager.h>

#include <Engine\Model.h>
#include <Engine\Material.h>
#include <Engine\ModelFactory.h>
#include <Engine\MaterialFactory.h>

MeshRendererComponent::MeshRendererComponent()
{
}

MeshRendererComponent::~MeshRendererComponent()
{
}

void MeshRendererComponent::Start()
{
	Expose(myModelPath, "Model");
	Expose(myMaterialPath, "Material");
	myTransformRef = GameObject().GetComponent<Transform>();
	myModelPath.SetString("Assets\\Models\\Chest\\Particle_Chest.erc");
	myMaterialPath.SetString("Assets\\Models\\Chest\\Chest.mat");
	Reload();
}

void MeshRendererComponent::Reload()
{
	if (myModelPath.GetString().size() < 5)
	{
		return;
	}

	myModel = Singleton<SE::ModelFactory>().GetModel(myModelPath.GetString());

	auto& factory = Singleton<SE::MaterialFactory>();
	const std::string matPath = myMaterialPath.GetString();
	const SE::Material mat = factory.CreateMaterial(matPath);
	myMaterial = std::make_shared<SE::Material>(mat);
}

void MeshRendererComponent::Render()
{
	if (!myModel || !myMaterial)
	{
		return;
	}

	float4x4 transform = myTransformRef->GetTransform();
	std::vector<float4x4> pose; // get from animator component if there is one
	SE::RenderSkinnedMeshCommand skmc(myModel, *myMaterial, transform, pose);
	Singleton<SE::RenderCommandManager>().RegisterCommand(skmc);
}
