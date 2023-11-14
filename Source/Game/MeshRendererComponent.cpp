#include "pch.h"
#include "MeshRendererComponent.h"
#include "Transform.h"
#include "Entity.h"
#include "MaterialComponent.h"
#include <Engine\RenderCommand.h>
#include <Engine\RenderCommandManager.h>
#include <Engine\Model.h>
#include <Engine\Material.h>
#include <Engine\ModelFactory.h>

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
}

void MeshRendererComponent::Reload()
{
	myModel = Singleton<SE::ModelFactory>().GetModel(myModelPath.GetString());
	// TODO: Create material factory to get material here as well
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
