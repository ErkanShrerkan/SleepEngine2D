#include "pch.h"
#include "DeferredRenderer.h"
#include "RenderCommandManager.h"

#include "Engine.h"

namespace SE
{
	CDeferredRenderer::CDeferredRenderer()
	{
	}

	CDeferredRenderer::~CDeferredRenderer()
	{
		if (myFrameBuffer)
		{
			myFrameBuffer->Release();
		}

		if (myObjectBuffer)
		{
			myObjectBuffer->Release();
		}
	}

	bool CDeferredRenderer::Init()
	{
		HRESULT result;

		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		ID3D11Device* device = CEngine::GetInstance()->GetDXDevice();

		bufferDesc.ByteWidth = sizeof(FrameBufferData);
		result = device->CreateBuffer(&bufferDesc, nullptr, &myFrameBuffer);
		if (FAILED(result))
		{
			return false;
		}

		bufferDesc.ByteWidth = sizeof(ObjectBufferData);
		result = device->CreateBuffer(&bufferDesc, nullptr, &myObjectBuffer);

		if (FAILED(result))
		{
			return false;
		}

		return true;
	}

	void CDeferredRenderer::GenerateGBuffer()
	{
		auto& smrc = Singleton<RenderCommandManager>().GetSkinnedMeshRenderCommands();
		smrc;
	}
}