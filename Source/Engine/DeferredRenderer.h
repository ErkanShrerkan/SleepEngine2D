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

		void GenerateGBuffer();

	private:

		ID3D11Buffer* myFrameBuffer = nullptr;
		ID3D11Buffer* myObjectBuffer = nullptr;
	};
}

