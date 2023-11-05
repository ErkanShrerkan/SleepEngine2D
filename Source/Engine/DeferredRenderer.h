#pragma once
#include <d3d11.h>
#include "Scene.h"
#include "RenderCommand.h"

namespace SE
{
	class CDeferredRenderer
	{
	public:
		CDeferredRenderer();
		~CDeferredRenderer();

		bool Init();

		void GenerateGBuffer(const std::vector<RenderCommand>& someRenderCommands);

	private:
		struct ObjectBufferData
		{
			Matrix4x4f myToWorld;
			float4 myScale;
			unsigned int myHasBones;
			unsigned int myNumBones;
			int x, y;
			Matrix4x4f myBones[128];
			float4 myColor;
		} myObjectBufferData;

		ID3D11Buffer* myFrameBuffer = nullptr;
		ID3D11Buffer* myObjectBuffer = nullptr;
	};
}

