#pragma once
#include <dxgiformat.h>
#include <unordered_map>
#include <queue>
#include "FullscreenTexture.h"
#include "GBuffer.h"

namespace SE
{
	class RenderTarget
	{
		friend class RenderTargetPool;
	public:
		template <class T>
		T& GetRenderTarget()
		{
			return *(T*)myResource;
		}

		void Return()
		{
			myIsAvailable = true;
		}

		bool IsAvailable()
		{
			return myIsAvailable;
		}

		void Release()
		{
			myGBuffer.Release();
			myFullscreenTexture.Release();
		}

		void Clear()
		{
			myGBuffer.ClearTextures();
			myFullscreenTexture.ClearTexture();
			myFullscreenTexture.ClearDepth();
		}

	private:
		void InitRenderTarget(const std::string& aFormat);

		void Borrow()
		{
			myIsAvailable = false;
		}

	private:
		bool myIsAvailable = true;
		void* myResource = nullptr;
		CGBuffer myGBuffer;
		CFullscreenTexture myFullscreenTexture;
	};

	class RenderTargetPool
	{
	public:
		RenderTarget& Borrow()
		{
			for (auto& t : myTargets)
			{
				if (t.IsAvailable())
				{
					t.Borrow();
					return t;
				}
			}

			RenderTarget& rt = CreateNewRenderTarget();
			rt.Borrow();
			rt.InitRenderTarget(myFormat);
			return rt;
		}

		void SetFormat(const std::string& aFormat)
		{
			myFormat = aFormat;
		}

		void ReleaseAll()
		{
			for (auto& t : myTargets)
			{
				t.Release();
			}

			myTargets.clear();
		}

		void ClearAll()
		{
			for (auto& t : myTargets)
			{
				t.Clear();
			}
		}

	private:
		RenderTarget& CreateNewRenderTarget()
		{
			myTargets.push_back(RenderTarget());
			return myTargets[myTargets.size() - 1];
		}

	private:
		std::string myFormat;
		std::vector<RenderTarget> myTargets;
	};

	class RenderTargetManager
	{
	public:
		~RenderTargetManager();

		RenderTarget& GetFullscreenDepth(uint2 aRes);
		RenderTarget& GetFullscreenTexture(uint2 aRes, DXGI_FORMAT aFormat);
		RenderTarget& GetGBuffer(uint2 aRes, DXGI_FORMAT aFormat);
		void ReleaseAllTextures();
		void ClearAllTextures();
		void Update();

	private:
		RenderTarget& GetRenderTarget(const std::string& aFormat);
		std::string GetFormat(uint2 aRes, DXGI_FORMAT aFormat);

	private:
		std::unordered_map<std::string, RenderTargetPool> myRenderTargetPools;
	};

}
