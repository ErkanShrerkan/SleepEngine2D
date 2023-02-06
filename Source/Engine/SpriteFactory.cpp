#include "pch.h"
#include "SpriteFactory.h"

#include <Engine/Engine.h>
#include <Engine/ContentLoader.h>
#include "Sprite.h"
#include "Texture.h"
#include "ShaderHelper.h"
#include "TextureFactory.h"
#include <Game\Globals.h>

#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <fstream>
#include <algorithm>

namespace SE
{
	void CSpriteFactory::InitFactory()
	{
		myHasInited = true;

		HRESULT result;
		ID3D11Device* const& device = CEngine::GetInstance()->GetDXDevice();

		uint indices[6] =
		{
			2, 1, 0,
			0, 3, 2
		};

		D3D11_BUFFER_DESC indexBufferDescription = { 0 };
		indexBufferDescription.ByteWidth = sizeof(indices);
		indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexSubresourceData = { 0 };
		indexSubresourceData.pSysMem = indices;

		result = device->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &myIndexBuffer);
		if (FAILED(result))
		{
			/* Error Message */
			assert(!"<SE::CSpriteFactory::InitFactory> Error creating IndexBuffer");
		}
		// End Vertex

		// Start Shader
		std::string vsData;
		if (!Helper::ShaderHelper::CreateVertexShader(&myVertexShader, "Shaders/SpriteVertexShader", &vsData))
		{
			/* Error Message */
			assert(!"<SE::CSpriteFactory::InitFactory> Error creating VertexShader");
		}

		// Start Layout
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV",		  0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		result = device->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &myInputLayout);
		if (FAILED(result))
		{
			/* Error Message */
			assert(!"<SE::CSpriteFactory::InitFactory> Error creating InputLayout");
		}

		float halfWidth = 1;
		float halfHeight = 1;
		SVertex vertices[4] = {
			//         X            Y    Z    W         R    G    B    A         U    V
			{  halfWidth,  halfHeight, 0.5f, 1.f, /**/ 1.f, 1.f, 1.f, 1.f, /**/ 1.f, 1.f },
			{  halfWidth, -halfHeight, 0.5f, 1.f, /**/ 1.f, 1.f, 1.f, 1.f, /**/ 1.f, 0.f },
			{ -halfWidth, -halfHeight, 0.5f, 1.f, /**/ 1.f, 1.f, 1.f, 1.f, /**/ 0.f, 0.f },
			{ -halfWidth,  halfHeight, 0.5f, 1.f, /**/ 1.f, 1.f, 1.f, 1.f, /**/ 0.f, 1.f },
		};

		D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
		vertexBufferDescription.ByteWidth = sizeof(vertices);
		vertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA vertexSubresourceData = { 0 };
		vertexSubresourceData.pSysMem = vertices;

		result = device->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &myVertexBuffer);
		if (FAILED(result))
		{
			/* Error Message */
			assert(!"<SE::CSpriteFactory::LoadSprite> Error creating VertexBuffer");
		}

		Singleton<CSprite::Data>().myVertexBuffer = myVertexBuffer;
		Singleton<CSprite::Data>().myVertexShader = myVertexShader;
		Singleton<CSprite::Data>().myIndexBuffer = myIndexBuffer;
		Singleton<CSprite::Data>().myInputLayout = myInputLayout;
		Singleton<CSprite::Data>().myStride = sizeof(SVertex);
		Singleton<CSprite::Data>().myOffset = 0u;
	}

	SE::CSprite* CSpriteFactory::GetSprite(const std::string& aSpritePath)
	{
		auto sprite = CreateSprite(aSpritePath);
		mySprites.insert(sprite);
		return sprite;
	}

	void CSpriteFactory::ReleaseSprite(CSprite* aSprite)
	{
		aSprite;
		if (mySprites.find(aSprite) != mySprites.end())
		{
			mySprites.erase(aSprite);
			delete aSprite;
			aSprite = nullptr;
		}
	}

	CSpriteFactory::~CSpriteFactory()
	{
		myVertexShader->Release();
		myVertexBuffer->Release();
		myIndexBuffer->Release();
		myInputLayout->Release();

		std::vector<CSprite*> sprites;
		for (auto& sprite : mySprites)
		{
			sprites.push_back(sprite);
		}

		for (auto& sprite : sprites)
		{
			delete sprite;
		}
	}

	SE::CSprite* CSpriteFactory::CreateSprite(const std::string& aSpritePath)
	{
		if (!myHasInited)
			InitFactory();

		CTexture* texture = Singleton<CTextureFactory>().LoadTexture(aSpritePath.c_str());
		CTexture* mask = Singleton<CTextureFactory>().LoadTexture("textures/maskdefault.dds");

		uint2 res = Singleton<GlobalSettings>().gameplayResolution;

		CSprite* sprite = new CSprite();
		sprite->mySize = { texture->GetWidth() / res.x, texture->GetHeight() / res.x };
		sprite->myTexture = texture;
		sprite->myMaskTexture = mask;
		sprite->myPivot = { 0, 0 };
		sprite->myRect = { 0, 0, 1, 1 };
		return sprite;
	}
}
