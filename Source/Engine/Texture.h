#pragma once
#include <d3d11.h>
//#include <string_view>
//#include "AutoReleaser.h"

struct ID3D11ShaderResourceView;
namespace SE
{
	class CTextureResource
	{
	public:
		CTextureResource()
			: srv(nullptr)
			, width(0)
			, height(0)
			, mipLevels(0)
			, format(0u)
			, loaded(false)
		{}

		~CTextureResource()
		{
			while (!loaded)
			{
				Sleep(1);
			}

			if (srv)
			{
				srv->Release();
			}
		}

	public:
		bool loaded;
		unsigned format;
		float width, height, mipLevels;
		ID3D11ShaderResourceView* srv;
		std::string filePath;
	};

	struct STextureResourcePtr
	{
		sptr(CTextureResource) ptr;
	};

	class CTexture
	{
	public:
		CTexture(sptr(STextureResourcePtr) aPtr);
		~CTexture();
		ID3D11ShaderResourceView* const GetShaderResourceView() const noexcept;
		ID3D11ShaderResourceView* const* GetPointerToShaderResourceView() const;
		
		inline const float GetWidth() const noexcept
		{
			return myResource->ptr->width;
		}
		inline const float GetHeight() const noexcept
		{
			return myResource->ptr->height;
		}
		inline const float GetMipLevels() const noexcept
		{
			return myResource->ptr->mipLevels;
		}

		void WaitUntilResourceIsLoaded()
		{
			if (ResourceIsLoaded())
				return;

			bool loaded = false;
			while (!loaded)
			{
				Sleep(1);
				if (ResourceIsLoaded())
				{
					loaded = true;
				}
			}
		}

	private:
		const bool ResourceIsLoaded() const
		{
			return myResource->ptr.get() && myResource->ptr->loaded;
		}

	private:
		sptr(STextureResourcePtr) myResource;
	};
}
