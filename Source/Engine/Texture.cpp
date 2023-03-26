#include "pch.h"
#include "Texture.h"
#include "TextureHelper.h"
#include <d3d11.h>
#include <Engine\Async.h>
#include <Engine\Engine.h>

SE::CTexture::CTexture(sptr(STextureResourcePtr) aPtr)
{
	myResource = aPtr;
}

SE::CTexture::~CTexture()
{
}

ID3D11ShaderResourceView* const SE::CTexture::GetShaderResourceView() const noexcept
{
	if (myResource->ptr)
	{
		return myResource->ptr->srv;// .Raw();
	}
	else
	{
		return nullptr;
	}
}

ID3D11ShaderResourceView* const* SE::CTexture::GetPointerToShaderResourceView() const
{
	if (myResource->ptr)
	{
		return &myResource->ptr->srv;// .Raw();
	}
	else
	{
		return nullptr;
	}
}
