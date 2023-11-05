#include "pch.h"
#include "RenderCommand.h"


namespace SE
{

	RenderSkinnedMeshCommand::RenderSkinnedMeshCommand(
		Model* aMesh,
		Material aMaterial,
		float4x4 aTransform,
		std::vector<float4x4> aPose)
		: myMesh(aMesh), myMaterial(aMaterial), myTransform(aTransform), myPose(aPose)
	{
	}

	void RenderSkinnedMeshCommand::Execute()
	{
	}

}