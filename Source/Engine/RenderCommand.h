#pragma once
#include <Game\Transform.h>

namespace SE
{
	class Mesh;
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

	class RenderSkinnedMeshCommand
	{
		RenderSkinnedMeshCommand(Mesh* aMesh, Material* aMaterial, Transform* aTransform);
	};

	class RenderMeshCommand
	{

	};
}

