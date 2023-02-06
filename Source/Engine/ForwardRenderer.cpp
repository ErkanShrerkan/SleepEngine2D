#include "pch.h"
#include "ForwardRenderer.h"
#include "DirectX11Framework.h"

enum ESlot {
	ESlot_SR_CubeMap,
	ESlot_SR_Textures,
};

#include <Game\CameraComponent.h>
#include <Game\Entity.h>
#include <Game\Transform.h>
#include "ShaderHelper.h"
#include <Game\Sprite.h>
#include "Sprite.h"
#include "Texture.h"
#include "Engine.h"
#include "DebugProfiler.h"
#include <d3d11.h>

namespace SE
{
	CForwardRenderer::CForwardRenderer()
		: myContext(nullptr)
	{}

	CForwardRenderer::~CForwardRenderer()
	{
		if (myFisheyePixelShader)
		{
			myFisheyePixelShader->Release();
			myCircularFillPixelShader->Release();
			mySpritePixelShader->Release();
			mySpriteVertexShader->Release();
		}
	}

	bool CForwardRenderer::Init(CDirectX11Framework* aFramework)
	{
		if (!aFramework)
		{
			/* Error Message */
			return false;
		}

		myContext = aFramework->GetContext();
		if (!myContext)
		{
			/* Error Message */
			return false;
		}

		ID3D11Device* device = aFramework->GetDevice();

		HRESULT result;
		D3D11_BUFFER_DESC bufferDescription = { 0 };
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		bufferDescription.ByteWidth = sizeof(SFrameBufferData);
		result = device->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
		if (FAILED(result))
		{
			/* Error message */
			return false;
		}

		bufferDescription.ByteWidth = sizeof(SSpriteBufferData);
		result = device->CreateBuffer(&bufferDescription, nullptr, &mySpriteBuffer);
		if (FAILED(result))
		{
			/* Error message */
			return false;
		}

		Helper::ShaderHelper::CreatePixelShader(&mySpritePixelShader, "Shaders/SpritePixelShader");
		Helper::ShaderHelper::CreateVertexShader(&mySpriteVertexShader, "Shaders/SpriteVertexShader");
		Helper::ShaderHelper::CreatePixelShader(&myFisheyePixelShader, "Shaders/SpritePS-Fisheye");
		Helper::ShaderHelper::CreatePixelShader(&myCircularFillPixelShader, "Shaders/SpritePS-CircularFill");

		return true;
	}

	void CForwardRenderer::RenderSprites(CameraComponent* aCamera, CommonUtilities::RefillVector<Sprite*>& someSprites)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE bufferData = { 0 };

		myContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myContext->VSSetShader(mySpriteVertexShader, nullptr, 0u);
		myContext->IASetInputLayout(Singleton<CSprite::Data>().myInputLayout);
		myContext->IASetIndexBuffer(Singleton<CSprite::Data>().myIndexBuffer, DXGI_FORMAT_R32_UINT, 0u);
		myContext->IASetVertexBuffers(0u, 1u, &Singleton<CSprite::Data>().myVertexBuffer, &Singleton<CSprite::Data>().myStride, &Singleton<CSprite::Data>().myOffset);

		float4x4 cameraTransform = aCamera->GameObject().GetComponent<Transform>().GetMatrix();
		myFrameBufferData.myCameraTransform = cameraTransform;
		myFrameBufferData.myToCamera = float4x4::GetFastInverse(myFrameBufferData.myCameraTransform);
		myFrameBufferData.myToProjection = aCamera->GetProjection();
		myFrameBufferData.myCameraPosition = cameraTransform.GetRow(4);

		ID3D11DeviceContext* context = CEngine::GetInstance()->GetDXDeviceContext();

		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = context->Map(myFrameBuffer.Raw(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			return;
		}

		memcpy(bufferData.pData, &myFrameBufferData, sizeof(SFrameBufferData));
		context->Unmap(myFrameBuffer.Raw(), 0);
		context->VSSetConstantBuffers(0, 1, &myFrameBuffer.Raw());
		context->PSSetConstantBuffers(0, 1, &myFrameBuffer.Raw());
		context->GSSetConstantBuffers(0, 1, &myFrameBuffer.Raw());

		ID3D11ShaderResourceView* lastTexture = nullptr;
		ID3D11ShaderResourceView* lastMask = nullptr;
		for (auto& sprite : someSprites)
		{
			mySpriteBufferData.myPosition = sprite->GetPosition();
			mySpriteBufferData.mySize = sprite->GetSize();
			mySpriteBufferData.myPivot = sprite->GetPivot();
			mySpriteBufferData.myRotation = Math::DegreeToRadian(sprite->GetRotation());
			mySpriteBufferData.myRect = sprite->GetRect();
			mySpriteBufferData.myData = sprite->GetShaderData();
			mySpriteBufferData.myColor = sprite->GetColor();

			ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
			result = myContext->Map(mySpriteBuffer.Raw(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
			if (FAILED(result))
			{
				/* Error Message */
				return;
			}
			memcpy(bufferData.pData, &mySpriteBufferData, sizeof(SSpriteBufferData));
			myContext->Unmap(mySpriteBuffer.Raw(), 0);

			ID3D11ShaderResourceView* texture = *sprite->GetSprite()->myTexture->GetPointerToShaderResourceView();
			ID3D11ShaderResourceView* mask = *sprite->GetSprite()->myMaskTexture->GetPointerToShaderResourceView();
			if (lastTexture != texture)
			{
				myContext->PSSetShaderResources(0u, 1u, &texture);
			}
			if (lastMask != mask)
			{
				myContext->PSSetShaderResources(1u, 1u, &mask);
			}
			lastTexture = texture;
			lastMask = mask;

			myContext->PSSetConstantBuffers(0u, 1u, &mySpriteBuffer);
			myContext->VSSetConstantBuffers(0u, 1u, &mySpriteBuffer);

			ID3D11PixelShader* pixelShader;

			switch (sprite->GetShaderType())
			{
			case SE::SpriteShaderType::eNone:
				pixelShader = mySpritePixelShader;
				break;
			case SE::SpriteShaderType::eCircularFill:
				pixelShader = myCircularFillPixelShader;
				break;
			case SE::SpriteShaderType::eFisheye:
				pixelShader = myFisheyePixelShader;
				break;
			default:
				pixelShader = mySpritePixelShader;
				break;
			}

			myContext->PSSetShader(pixelShader, nullptr, 0u);

			myContext->DrawIndexed(6u, 0u, 0u);
			Singleton<Debug::CDebugProfiler>().IncrementDrawCallCount();
		}
	}

	// Renders screen space sprites
	void CForwardRenderer::RenderSprites(CameraComponent* aCamera, CommonUtilities::RefillVector<CSprite*>& someSprites)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE bufferData = { 0 };

		myContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myContext->VSSetShader(mySpriteVertexShader, nullptr, 0u);
		myContext->IASetInputLayout(Singleton<CSprite::Data>().myInputLayout);
		myContext->IASetIndexBuffer(Singleton<CSprite::Data>().myIndexBuffer, DXGI_FORMAT_R32_UINT, 0u);
		myContext->IASetVertexBuffers(0u, 1u, &Singleton<CSprite::Data>().myVertexBuffer, &Singleton<CSprite::Data>().myStride, &Singleton<CSprite::Data>().myOffset);

		float4x4 cameraTransform = aCamera->GameObject().GetComponent<Transform>().GetMatrix();
		myFrameBufferData.myCameraTransform = cameraTransform;
		myFrameBufferData.myToCamera = float4x4::GetFastInverse(myFrameBufferData.myCameraTransform);
		myFrameBufferData.myToProjection = aCamera->GetProjection();
		myFrameBufferData.myCameraPosition = cameraTransform.GetRow(4);

		ID3D11DeviceContext* context = CEngine::GetInstance()->GetDXDeviceContext();

		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = context->Map(myFrameBuffer.Raw(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			return;
		}

		memcpy(bufferData.pData, &myFrameBufferData, sizeof(SFrameBufferData));
		context->Unmap(myFrameBuffer.Raw(), 0);
		context->VSSetConstantBuffers(0, 1, &myFrameBuffer.Raw());
		context->PSSetConstantBuffers(0, 1, &myFrameBuffer.Raw());
		context->GSSetConstantBuffers(0, 1, &myFrameBuffer.Raw());

		ID3D11ShaderResourceView* lastTexture = nullptr;
		ID3D11ShaderResourceView* lastMask = nullptr;
		for (auto& sprite : someSprites)
		{
			mySpriteBufferData.myPosition = sprite->GetPosition();
			mySpriteBufferData.mySize = sprite->GetSize();
			mySpriteBufferData.myPivot = sprite->GetPivot();
			mySpriteBufferData.myRotation = Math::DegreeToRadian(sprite->GetRotation());
			mySpriteBufferData.myRect = sprite->GetRect();
			mySpriteBufferData.myData = sprite->GetShaderData();
			mySpriteBufferData.myColor = sprite->GetColor();

			ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
			result = myContext->Map(mySpriteBuffer.Raw(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
			if (FAILED(result))
			{
				/* Error Message */
				return;
			}
			memcpy(bufferData.pData, &mySpriteBufferData, sizeof(SSpriteBufferData));
			myContext->Unmap(mySpriteBuffer.Raw(), 0);

			ID3D11ShaderResourceView* texture = *sprite->myTexture->GetPointerToShaderResourceView();
			ID3D11ShaderResourceView* mask = *sprite->myMaskTexture->GetPointerToShaderResourceView();
			if (lastTexture != texture)
			{
				myContext->PSSetShaderResources(0u, 1u, &texture);
			}
			if (lastMask != mask)
			{
				myContext->PSSetShaderResources(1u, 1u, &mask);
			}
			lastTexture = texture;
			lastMask = mask;

			myContext->PSSetConstantBuffers(0u, 1u, &mySpriteBuffer);
			myContext->VSSetConstantBuffers(0u, 1u, &mySpriteBuffer);

			ID3D11PixelShader* pixelShader;

			switch (sprite->GetShaderType())
			{
			case SE::SpriteShaderType::eNone:
				pixelShader = mySpritePixelShader;
				break;
			case SE::SpriteShaderType::eCircularFill:
				pixelShader = myCircularFillPixelShader;
				break;
			case SE::SpriteShaderType::eFisheye:
				pixelShader = myFisheyePixelShader;
				break;
			default:
				pixelShader = mySpritePixelShader;
				break;
			}

			myContext->PSSetShader(pixelShader, nullptr, 0u);

			myContext->DrawIndexed(6u, 0u, 0u);
			Singleton<Debug::CDebugProfiler>().IncrementDrawCallCount();
		}
	}
}
