#include "pch.h"
#include "Texture.h"
#include "TextureHelper.h"
#include <d3d11.h>
#include <Engine\Async.h>
#include <Engine\Engine.h>

SE::CTexture::CTexture()
	: myShaderResourceView(nullptr)
	, myWidth(0)
	, myHeight(0)
	, myMipLevels(0)
	, myFormat(0u)
{}

SE::CTexture::CTexture(const std::string& aFilePath)
	: myShaderResourceView(nullptr)
	, myWidth(0)
	, myHeight(0)
	, myMipLevels(0)
	, myFormat(0u)
	, myFilePath(aFilePath)
{
	LoadResource();
}

SE::CTexture::CTexture(std::string& aFilePath)
	: myShaderResourceView(nullptr)
	, myWidth(0)
	, myHeight(0)
	, myMipLevels(0)
	, myFormat(0u)
	, myFilePath(aFilePath)
{
	LoadResource();
}

SE::CTexture::~CTexture()
{
	while (!myLoaded)
	{
		Sleep(1);
	}

	if (myShaderResourceView)
	{
		myShaderResourceView->Release();
		//printf("Texture [%s] deleted\n", myFilePath.c_str());
	}
}

ID3D11ShaderResourceView* const SE::CTexture::GetShaderResourceView() const noexcept
{
	return myShaderResourceView;// .Raw();
}

ID3D11ShaderResourceView* const* SE::CTexture::GetPointerToShaderResourceView() const
{
	return &myShaderResourceView;
}

void SE::CTexture::SetShaderResourceView(ID3D11ShaderResourceView* const aShaderResourceView)
{
	myShaderResourceView = aShaderResourceView;
}

ID3D11ShaderResourceView* SE::CTexture::StealShaderResource()
{
	ID3D11ShaderResourceView* oldPointer = myShaderResourceView;
	myShaderResourceView = nullptr;

	oldPointer->AddRef();
	return oldPointer;
}

void SE::CTexture::LoadResource()
{
	Async<void> texLoader([&]
		{
			Helper::TextureHelper::LoadShaderResourceView(&myShaderResourceView, myFilePath);
	
			if (myShaderResourceView == nullptr)
			{
				/* Error Message */
				perr("Error loading \"%s\" as a CTexture", myFilePath.c_str());
				myFilePath = "Assets/Textures/Error/Albedo_c.dds";
				Helper::TextureHelper::LoadShaderResourceView(&myShaderResourceView, myFilePath);
			}
	
			// Get size of image
			ID3D11Resource* resource = nullptr;
			myShaderResourceView->GetResource(&resource);
	
			D3D11_TEXTURE2D_DESC description;
			reinterpret_cast<ID3D11Texture2D*>(resource)->GetDesc(&description);
			resource->Release();
	
			myWidth = static_cast<float>(description.Width);
			myHeight = static_cast<float>(description.Height);
			myMipLevels = static_cast<float>(description.MipLevels);
			myFormat = static_cast<unsigned>(description.Format);
	
			if (!(Math::IsPowerOfTwo(description.Width) && Math::IsPowerOfTwo(description.Height)))
			{
				pwarn("Texture is not power of two (%.fx%.f) \"%s\"", myWidth, myHeight, myFilePath.c_str());
			}
	
			myLoaded = true;
		});
}
