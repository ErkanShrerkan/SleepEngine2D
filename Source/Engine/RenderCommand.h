#pragma once
#include "Material.h"

namespace SE
{
	class Model;

	class RenderCommand
	{
	public:
		virtual void Execute() = 0;
		virtual ~RenderCommand() = default;
	};

	class RenderStaticMeshCommand
	{

	};

	class RenderSkinnedMeshCommand
	{
	public:
		RenderSkinnedMeshCommand(
			Model* aMesh, 
			Material aMaterial,
			float4x4 aTransform,
			std::vector<float4x4> aPose
		);

		Model* GetModel() { return myModel; }
		Material& GetMaterial() { return myMaterial; }
		float4x4& GetTransform() { return myTransform; }
		bool IsAnimated() { return myIsAnimated; }
		std::vector<float4x4>& GetPose() { return myPose; }

	private:
		bool myIsAnimated = false;
		Model* myModel;
		Material myMaterial;
		float4x4 myTransform;
		std::vector<float4x4> myPose;
	};

	class RenderMeshCommand
	{

	};
}

