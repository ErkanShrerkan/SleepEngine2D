#include "pch.h"
#include "MeshRendererComponent.h"
#include "MeshComponent.h"
#include "Transform.h"
#include "MaterialComponent.h"
#include <Engine\RenderCommand.h>
#include <Engine\RenderCommandManager.h>
#include <Engine\Model.h>

MeshRendererComponent::MeshRendererComponent()
{
}

MeshRendererComponent::~MeshRendererComponent()
{
}

void MeshRendererComponent::Start()
{
	Expose(myMeshRef, "Mesh To Render");
	Expose(myMatRef, "Material");
	Expose(myTransformRef, "Transform");
}

void MeshRendererComponent::Render()
{
	if (!myMeshRef || !myMatRef || !myTransformRef)
	{
		return;
	}

	SE::Model* mesh = myMeshRef->GetMesh();
	SE::Material mat = myMatRef->GetMaterial();
	float4x4 transform = myTransformRef->GetTransform();
	std::vector<float4x4> pose;
	SE::RenderSkinnedMeshCommand skmc(mesh, mat, transform, pose);
	Singleton<SE::RenderCommandManager>().RegisterCommand(skmc);
}
