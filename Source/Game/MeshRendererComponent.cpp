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
	if (myMaterial)
	{
		myMaterial->Release();
	}
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
	
	if (myMaterial)
	{
		myMaterial->Release();
		delete myMaterial;
		myMaterial = nullptr;
	}

	myMaterial = new SE::Material(Singleton<SE::MaterialFactory>().CreateMaterial(myMaterialPath.GetString()));
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
