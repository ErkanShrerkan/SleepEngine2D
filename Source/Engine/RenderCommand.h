#pragma once
#include "Material.h"

namespace SE
{
	class Model;
	class Material;

	class RenderCommand
	{
	public:
		virtual void Execute() = 0;
		virtual ~RenderCommand() = default;
	};

	class RenderStaticMeshCommand
	{

	};

	class RenderSkinnedMeshCommand : public RenderCommand
	{
	public:
		RenderSkinnedMeshCommand(
			Model* aMesh, 
			Material aMaterial,
			float4x4 aTransform,
			std::vector<float4x4> aPose
		);

		virtual void Execute() override;

	private:
		Model* myMesh;
		Material myMaterial;
		float4x4 myTransform;
		std::vector<float4x4> myPose;
	};

	class RenderMeshCommand
	{

	};
}

