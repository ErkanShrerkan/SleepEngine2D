#include "pch.h"
#include "Texture.h"
#include "TextureHelper.h"
#include <d3d11.h>
#include <Engine\Async.h>
#include <Engine\Engine.h>

SE::Texture::Texture(sptr(STextureResourcePtr) aPtr)
{
	myResource = aPtr;
}

SE::Texture::~Texture()
{
}

ID3D11ShaderResourceView* const SE::Texture::GetShaderResourceView() const noexcept
{
	if (myResource->ptr)
	{
		return myResource->ptr->srv;
	}
	else
	{
		return nullptr;
	}
}

ID3D11ShaderResourceView* const* SE::Texture::GetPointerToShaderResourceView() const
{
	if (myResource->ptr)
	{
		return &myResource->ptr->srv;
	}
	else
	{
		return nullptr;
	}
}

SE::CTextureResource::~CTextureResource()
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
