#include "pch.h"
#include "DeferredRenderer.h"
#include "RenderCommandManager.h"

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
		return true;
	}

	void CDeferredRenderer::GenerateGBuffer()
	{
		auto& smrc = Singleton<RenderCommandManager>().GetSkinnedMeshRenderCommands();
		smrc;
	}
}