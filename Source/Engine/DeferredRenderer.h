#pragma once
#include <d3d11.h>
#include "Scene.h"
#include "RenderCommand.h"

namespace SE
{
	class CDirectX11Framework;
	class CDeferredRenderer
	{
	public:
		CDeferredRenderer();
		~CDeferredRenderer();

		bool Init(CDirectX11Framework* aFramework);

		bool BindDataToBuffer(ID3D11Buffer* aBuffer, void* aDataPtr, uint aDataSize);
		void GenerateGBuffer();

	private:

		ID3D11Buffer* myFrameBuffer = nullptr;
		ID3D11Buffer* myObjectBuffer = nullptr;
		ID3D11DeviceContext* myContext = nullptr;
	};
}

