#include "pch.h"
#include <d3d11.h>
#include "RenderTargetManager.h"
#include "TextureFactory.h"

namespace SE
{
	RenderTargetManager::~RenderTargetManager()
	{
		ReleaseAllTextures();
	}

	RenderTarget& RenderTargetManager::GetFullscreenTexture(uint2 aRes, DXGI_FORMAT aFormat)
	{
		std::string format = "GB_" + GetFormat(aRes, aFormat);
		return GetRenderTarget(format);
	}

	RenderTarget& RenderTargetManager::GetGBuffer(uint2 aRes, DXGI_FORMAT aFormat)
	{
		std::string format = "FT_" + GetFormat(aRes, aFormat);
		return GetRenderTarget(format);
	}

	void RenderTargetManager::ReleaseAllTextures()
	{
		for (auto& [format, target] : myRenderTargetPools)
		{
			target.ReleaseAll();
		}

		myRenderTargetPools.clear();
	}

	void RenderTargetManager::ClearAllTextures()
	{
		for (auto& [format, target] : myRenderTargetPools)
		{
			target.ClearAll();
		}
	}

	void RenderTargetManager::Update()
	{
	}

	RenderTarget& RenderTargetManager::GetRenderTarget(const std::string& aFormat)
	{
		auto& pool = myRenderTargetPools[aFormat];
		pool.SetFormat(aFormat);
		auto& renderTarget = pool.Borrow();
		return renderTarget;
	}

	std::string RenderTargetManager::GetFormat(uint2 aRes, DXGI_FORMAT aFormat)
	{
		std::string format = "";
		format += std::to_string(aRes.x);
		format += "_";
		format += std::to_string(aRes.y);
		format += "_";
		format += std::to_string(aFormat);
		return format;
	}

	void RenderTarget::InitRenderTarget(const std::string& aFormat)
	{
		std::string s = aFormat;
		size_t pos = 0;
		std::vector<std::string> data;
		std::string spacer = "_";
		while ((pos = s.find(spacer)) != std::string::npos)
		{
			data.push_back(s.substr(0, pos));
			s.erase(0, pos + spacer.length());
		}

		CTextureFactory& factory = Singleton<CTextureFactory>();

		uint2 res = { (uint)std::stoi(data[1]), (uint)std::stoi(data[2]) };

		DXGI_FORMAT format = (DXGI_FORMAT)std::stoi(data[3]);

		if (data[0] == "FT")
		{
			myFullscreenTexture = factory.CreateFullscreenTexture(res, format);
			myResource = &myFullscreenTexture;
		}
		else if (data[0] == "FD")
		{
			myFullscreenTexture = factory.CreateFullscreenDepth(res, format);
			myResource = &myFullscreenTexture;
		}
		else if (data[0] == "GB")
		{
			myGBuffer = factory.CreateGBuffer(res);
			myResource = &myGBuffer;
		}
	}
}