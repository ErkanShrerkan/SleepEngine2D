#include "pch.h"
#include "RenderCommand.h"


namespace SE
{

	RenderSkinnedMeshCommand::RenderSkinnedMeshCommand(
		Model* aMesh,
		Material aMaterial,
		float4x4 aTransform,
		std::vector<float4x4> aPose)
		: myModel(aMesh), myMaterial(aMaterial), myTransform(aTransform), myPose(aPose)
	{
		myIsAnimated = !aPose.empty();
	}

}