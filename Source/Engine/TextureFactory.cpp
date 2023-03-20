#include "pch.h"
#include "TextureFactory.h"
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include "Engine.h"
#include "FullscreenTexture.h"
#include "Texture.h"

#include "DX11.h"
#include <algorithm>

namespace SE
{
	CTextureFactory::~CTextureFactory()
	{
		myPool.clear();
	}

	CFullscreenTexture CTextureFactory::CreateFullscreenTexture(const Vector2ui& aSize, DXGI_FORMAT aFormat)
	{
		HRESULT result;
		D3D11_TEXTURE2D_DESC description = { 0 };
		description.Width = aSize.width;
		description.Height = aSize.height;
		description.MipLevels = 1u;
		description.ArraySize = 1u;
		description.Format = aFormat;
		description.SampleDesc.Count = 1u;
		description.SampleDesc.Quality = 0u;
		description.Usage = D3D11_USAGE_DEFAULT;
		description.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		description.CPUAccessFlags = 0u;
		description.MiscFlags = 0u;

		ID3D11Device* const& device = CEngine::GetInstance()->GetDXDevice();
		ID3D11Texture2D* texture = nullptr;
		result = device->CreateTexture2D(&description, nullptr, &texture);
		assert(SUCCEEDED(result));

		CFullscreenTexture textureResult = CreateFullscreenTexture(texture);

		ID3D11ShaderResourceView* shaderResourceView;
		result = device->CreateShaderResourceView(texture, nullptr, &shaderResourceView);
		assert(SUCCEEDED(result));

		textureResult.myShaderResourceView = shaderResourceView;
		return textureResult;
	}
	CFullscreenTexture CTextureFactory::CreateFullscreenDepth(const Vector2ui& aSize, DXGI_FORMAT aFormat)
	{
		HRESULT result;
		D3D11_TEXTURE2D_DESC description = { 0 };
		description.Width = aSize.width;
		description.Height = aSize.height;
		description.MipLevels = 1u;
		description.ArraySize = 1u;
		description.Format = aFormat;
		description.SampleDesc.Count = 1u;
		description.SampleDesc.Quality = 0u;
		description.Usage = D3D11_USAGE_DEFAULT;
		description.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		description.CPUAccessFlags = 0u;
		description.MiscFlags = 0u;

		CEngine* const& engine = CEngine::GetInstance();
		ID3D11Device* const& device = engine->GetDXDevice();

		ID3D11Texture2D* texture = nullptr;
		result = device->CreateTexture2D(&description, nullptr, &texture);
		assert(SUCCEEDED(result));

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		descDSV.Flags = 0;

		ID3D11DepthStencilView* depthStencilView;
		result = device->CreateDepthStencilView(texture, &descDSV, &depthStencilView);
		assert(SUCCEEDED(result));

		std::shared_ptr<D3D11_VIEWPORT> viewport = std::make_shared<D3D11_VIEWPORT>(D3D11_VIEWPORT{
			0.f, 0.f,
			static_cast<float>(aSize.width),
			static_cast<float>(aSize.height),
			0.f, 1.f
			});

		CFullscreenTexture textureResult;
		textureResult.myContext = engine->GetDXDeviceContext();
		textureResult.myTexture = texture;
		textureResult.myDepth = depthStencilView;
		textureResult.myViewport = viewport;
		return textureResult;
	}

	CFullscreenTexture CTextureFactory::CreateShadowMap(const Vector2ui& aSize)
	{
		HRESULT result;
		CEngine* const& engine = CEngine::GetInstance();
		ID3D11Device* const& device = engine->GetDXDevice();

		D3D11_TEXTURE2D_DESC description = { 0 };
		description.Width = aSize.width;
		description.Height = aSize.height;
		description.MipLevels = 1u;
		description.ArraySize = 1u;
		description.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		description.SampleDesc.Count = 1u;
		description.SampleDesc.Quality = 0u;
		description.Usage = D3D11_USAGE_DEFAULT;
		description.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		description.CPUAccessFlags = 0u;
		description.MiscFlags = 0u;

		ID3D11Texture2D* texture;
		result = device->CreateTexture2D(&description, nullptr, &texture);
		assert(SUCCEEDED(result));

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		ID3D11DepthStencilView* depth;
		result = device->CreateDepthStencilView(texture, &descDSV, &depth);
		assert(SUCCEEDED(result));

		D3D11_SHADER_RESOURCE_VIEW_DESC descSRV = {};
		descSRV.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		descSRV.Texture2D.MipLevels = description.MipLevels;

		ID3D11ShaderResourceView* srv;
		result = device->CreateShaderResourceView(texture, &descSRV, &srv);
		assert(SUCCEEDED(result));

		std::shared_ptr<D3D11_VIEWPORT> viewport = nullptr;
		viewport = std::make_shared<D3D11_VIEWPORT>(D3D11_VIEWPORT{
			0.f, 0.f,
			static_cast<float>(description.Width),
			static_cast<float>(description.Height),
			0.f, 1.f
			});

		CFullscreenTexture textureResult;
		textureResult.myTexture = texture;
		textureResult.myDepth = depth;
		textureResult.myShaderResourceView = srv;
		textureResult.myContext = engine->GetDXDeviceContext();
		textureResult.myViewport = viewport;
		return textureResult;
	}
	CFullscreenTexture CTextureFactory::CreateFullscreenTexture(ID3D11Texture2D* const& aTexture)
	{
		HRESULT result;
		CEngine* const& engine = CEngine::GetInstance();
		ID3D11Device* const& device = engine->GetDXDevice();

		ID3D11RenderTargetView* renderTargetView;
		result = device->CreateRenderTargetView(aTexture, nullptr, &renderTargetView);
		assert(SUCCEEDED(result));

		std::shared_ptr<D3D11_VIEWPORT> viewport = nullptr;
		if (aTexture)
		{
			D3D11_TEXTURE2D_DESC description;
			aTexture->GetDesc(&description);
			viewport = std::make_shared<D3D11_VIEWPORT>(D3D11_VIEWPORT{
				0.f, 0.f,
				static_cast<float>(description.Width),
				static_cast<float>(description.Height),
				0.f, 1.f
				});
		}

		CFullscreenTexture textureResult;
		textureResult.myContext = engine->GetDXDeviceContext();
		textureResult.myTexture = aTexture;
		textureResult.myRenderTargetView = renderTargetView;
		textureResult.myViewport = viewport;
		return textureResult;
	}

	CGBuffer CTextureFactory::CreateGBuffer(const Vector2ui& aSize)
	{
		HRESULT result;

		std::array<DXGI_FORMAT, CGBuffer::E_COUNT> textureFormats =
		{
			DXGI_FORMAT_R32G32B32A32_FLOAT, // pos
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, // albedo
			DXGI_FORMAT_R16G16B16A16_SNORM, // normal
			DXGI_FORMAT_R16G16B16A16_SNORM, // vertexnormal
			DXGI_FORMAT_R8G8B8A8_UNORM, // material
			DXGI_FORMAT_R8_UNORM, // ambientocclusion
			DXGI_FORMAT_R16_UNORM // depth
		};

		CGBuffer returnBuffer;

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = aSize.x;
		desc.Height = aSize.y;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		ID3D11Device* const& device = CEngine::GetInstance()->GetDXDevice();
		for (unsigned int idx = 0; idx < CGBuffer::E_COUNT; idx++)
		{
			desc.Format = textureFormats[idx];

			result = device->CreateTexture2D(&desc, nullptr, &returnBuffer.myTextures[idx]);
			assert(SUCCEEDED(result));

			result = device->CreateRenderTargetView(returnBuffer.myTextures[idx], nullptr, &returnBuffer.myRTVs[idx]);
			assert(SUCCEEDED(result));

			result = device->CreateShaderResourceView(returnBuffer.myTextures[idx], nullptr, &returnBuffer.mySRVs[idx]);
			assert(SUCCEEDED(result));
		}

		returnBuffer.myViewport = new D3D11_VIEWPORT(
			{
				0,
				0,
				static_cast<float>(desc.Width),
				static_cast<float>(desc.Height),
				0,
				1
			});

		return returnBuffer;
	}

	sptr(CTexture) CTextureFactory::LoadTexture(const std::string& aPath)
	{
		for (auto iter = myPool.begin(); iter != myPool.end();)
		{
			if (iter->second.expired())
			{
				printf("Resource [%s] expired\n", iter->first.c_str());
				iter = myPool.erase(iter);
			}
			else
			{
				++iter;
			}
		}

		std::string cleanName(aPath);
		std::replace(cleanName.begin(), cleanName.end(), '\\', '/');
#pragma warning(disable:4244)
		// '=': conversion from 'int' to 'char', possible loss of data
		std::string lowerCasePath = cleanName;
		std::transform(cleanName.begin(), cleanName.end(), lowerCasePath.begin(), std::tolower);
#pragma warning(default:4244)

		std::string modifiablePath = lowerCasePath;
		sptr(CTexture) texture;
		if (myPool.find(lowerCasePath) == myPool.end())
		{
			texture = CreateTexture(modifiablePath);
			myPool[modifiablePath] = texture;
			//printf("Resource [%s] created\n", lowerCasePath.c_str());
		}
		else
		{
			if (myPool[lowerCasePath].expired())
			{
				myPool.erase(lowerCasePath);
				texture = CreateTexture(modifiablePath);
				myPool[modifiablePath] = texture;
				//printf("Resource [%s] expired, creating it again\n", lowerCasePath.c_str());
			}
			else
			{
				texture = myPool[lowerCasePath].lock();
				//printf("Resource [%s] loaded\n", lowerCasePath.c_str());
			}
		}
		return texture;
	}
	sptr(CTexture) CTextureFactory::CreateTexture(std::string& aPath)
	{
		return std::make_shared<CTexture>(aPath);
	}
}
