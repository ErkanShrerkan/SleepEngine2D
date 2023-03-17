#include "pch.h"
#include "RenderManager.h"
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>

#include <array>
#include "Engine.h"

#include "DirectX11Framework.h"
#include "ContentLoader.h"

#include "DX11.h"
#include "TextureFactory.h"
#include "TextureHelper.h"

#include "Input.h"

#include "CDebugDrawer.h"
#include "EnvironmentLight.h"

#include "Scene.h"
#include "Sprite.h"
#include "SpriteFactory.h"
#include "Texture.h"

#include <algorithm>
#include <Game\Postmaster.h>
#include "LineDrawer.h"

#include <engine/Async.h>
#include <Game\macro.h>
#include <Game\Globals.h>
#include <Game\Sprite.h>
#include <Game\Entity.h>
#include <Game\GameManager.h>
#include <Game\CameraComponent.h>
#include <Game\EditorController.h>
#include <ImGui/imgui.h>

constexpr auto PI = 3.14159265f;

#define SKIP_SHADOW
#define SKIP_HD

namespace SE
{
	CRenderManager::~CRenderManager()
	{
		Postmaster::GetInstance().UnSubscribe(this, eMessage::eUpdateResolution);
		Postmaster::GetInstance().UnSubscribe(this, eMessage::eCullBack);
		Postmaster::GetInstance().UnSubscribe(this, eMessage::eCullFront);
		Postmaster::GetInstance().UnSubscribe(this, eMessage::eCullNone);

		myScaledBackBuffer.Release();
		myIntermediateDepth.Release();
		myFullscreenCopy.Release();
		myFullscreen.Release();
		myBackBuffer.Release();
		myGameWindow.Release();

		for (auto& state : myBlendStates)
		{
			if (state)
			{
				state->Release();
			}
		}

		for (auto& state : myDepthStencilStates)
		{
			if (state)
			{
				state->Release();
			}
		}

		for (auto& state : myRasterizerStates)
		{
			if (state)
			{
				state->Release();
			}
		}

		for (auto& state : mySamplerStates)
		{
			if (state)
			{
				state->Release();
			}
		}

		myPostProcessingBuffer->Release();
	}

	bool CRenderManager::Restart()
	{
		CreateTextures();
		return true;
	}

	//#define DELTA_TIME CEngine::GetInstance()->GetDeltaTime();
	bool CRenderManager::Init(CDirectX11Framework* aFramework)
	{
		myPostProcessingData.shallowWaterColor = float3(37.f / 255, 114.f / 255, 17.f / 255);
		myPostProcessingData.deepWaterColor = float3(24.f / 255, 47.f / 255, 8.f / 255);
		myPostProcessingData.waterBorderColor = float3(153.f / 255, 175.f / 255, 147.f / 255);
		//myPostProcessingData.depthMult = 10.f;
		//myPostProcessingData.alphaMult = 25.f;

		if (!myForwardRenderer.Init(aFramework))
		{
			/* Error Message */
			return false;
		}

		if (!myFullscreenRenderer.Init())
		{
			/* Error Message */
			return false;
		}

		CLineDrawer::Init();
		CDebugDrawer::GetInstance().Init();

		myCursor = CEngine::GetInstance()->GetContentLoader()->GetSpriteFactory().GetSprite(/*"textures/sprites/player/CharacterIdleFront.dds"*/"textures/cursor.dds");
		//myCursor->isEngineDependent = true;
		//myCursor->SetSizeRelativeToImage({ 640.f / 1920, 360.f / 1080 });
		myCursor->SetSizeRelativeToImage({ .05f, .05f * (16.f / 9) });
		ID3D11Resource* backBufferResource = nullptr;
		aFramework->GetBackBuffer()->GetResource(&backBufferResource);
		ID3D11Texture2D* backBufferTexture = reinterpret_cast<ID3D11Texture2D*>(backBufferResource);
		if (!backBufferTexture)
		{
			/* Error Message */
			return false;
		}

		HRESULT result;
		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		bufferDesc.ByteWidth = sizeof(PostProcessingData);
		result = CEngine::GetInstance()->GetDXDevice()->CreateBuffer(&bufferDesc, nullptr, &myPostProcessingBuffer);
		assert(SUCCEEDED(result));

		// settings
		ID3D11Device* device = CEngine::GetInstance()->GetDXDevice();

		D3D11_BLEND_DESC alphaBlendDesc = { };
		alphaBlendDesc.RenderTarget[0].BlendEnable = true;
		alphaBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		alphaBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		alphaBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		alphaBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		alphaBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		alphaBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		alphaBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = device->CreateBlendState(&alphaBlendDesc, &myBlendStates[E_BLENDSTATE_ALPHABLEND]);
		assert(SUCCEEDED(result));

		D3D11_BLEND_DESC additiveBlendDesc = { };
		additiveBlendDesc.RenderTarget[0].BlendEnable = true;
		additiveBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		additiveBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		additiveBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		additiveBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		additiveBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		additiveBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		additiveBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = device->CreateBlendState(&additiveBlendDesc, &myBlendStates[E_BLENDSTATE_ADDITIVE]);
		assert(SUCCEEDED(result));

		myBlendStates[E_BLENDSTATE_DISABLE] = nullptr;

		D3D11_DEPTH_STENCIL_DESC readOnlyDepthDesc = {};
		readOnlyDepthDesc.DepthEnable = true;
		readOnlyDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		readOnlyDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
		readOnlyDepthDesc.StencilEnable = false;

		result = device->CreateDepthStencilState(&readOnlyDepthDesc, &myDepthStencilStates[E_DEPTHSTENCILSTATE_READONLY]);
		assert(SUCCEEDED(result));

		myDepthStencilStates[E_DEPTHSTENCILSTATE_DEFAULT] = nullptr;

		D3D11_RASTERIZER_DESC wireframeRasterizerDesc = {};
		wireframeRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		wireframeRasterizerDesc.CullMode = D3D11_CULL_BACK;
		wireframeRasterizerDesc.DepthClipEnable = true;

		result = device->CreateRasterizerState(&wireframeRasterizerDesc, &myRasterizerStates[E_RASTERIZERSTATE_WIREFRAME]);
		assert(SUCCEEDED(result));

		D3D11_RASTERIZER_DESC frontDesc = {};
		frontDesc.FillMode = D3D11_FILL_SOLID;
		frontDesc.CullMode = D3D11_CULL_FRONT; // if below is true this has to be D3D11_CULL_BACK
		//negDesc.FrontCounterClockwise = true;
		frontDesc.DepthClipEnable = true;
		result = device->CreateRasterizerState(&frontDesc, &myRasterizerStates[E_RASTERIZERSTATE_CULLFRONT]);
		assert(SUCCEEDED(result));

		D3D11_RASTERIZER_DESC noneDesc = {};
		noneDesc.FillMode = D3D11_FILL_SOLID;
		noneDesc.CullMode = D3D11_CULL_NONE; // if below is true this has to be D3D11_CULL_BACK
		//negDesc.FrontCounterClockwise = true;
		noneDesc.DepthClipEnable = true;
		result = device->CreateRasterizerState(&noneDesc, &myRasterizerStates[E_RASTERIZERSTATE_CULLNONE]);
		assert(SUCCEEDED(result));

		D3D11_RASTERIZER_DESC def = {};
		def.FillMode = D3D11_FILL_SOLID;
		def.CullMode = D3D11_CULL_BACK;
		def.DepthClipEnable = true;
		result = device->CreateRasterizerState(&def, &myRasterizerStates[E_RASTERIZERSTATE_DEFAULT]);
		assert(SUCCEEDED(result));
		SetRasterizerState(ERasterizerState::E_RASTERIZERSTATE_DEFAULT);
		//myRasterizerStates[E_RASTERIZERSTATE_DEFAULT] = nullptr;

		D3D11_SAMPLER_DESC pointSampleDesc = {};
		pointSampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		pointSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		pointSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		pointSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		pointSampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		pointSampleDesc.MinLOD = -FLT_MAX;
		pointSampleDesc.MaxLOD = FLT_MAX;

		result = device->CreateSamplerState(&pointSampleDesc, &mySamplerStates[E_SAMPLERSTATE_POINT]);
		assert(SUCCEEDED(result));

		D3D11_SAMPLER_DESC trilWrapSampleDesc = {};
		trilWrapSampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		trilWrapSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		trilWrapSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		trilWrapSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		trilWrapSampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		trilWrapSampleDesc.MinLOD = -FLT_MAX;
		trilWrapSampleDesc.MaxLOD = FLT_MAX;

		result = device->CreateSamplerState(&trilWrapSampleDesc, &mySamplerStates[E_SAMPLERSTATE_TRILINEAR_WRAP]);
		assert(SUCCEEDED(result));


		D3D11_SAMPLER_DESC aniFilSampleDesc = {};
		aniFilSampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		aniFilSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		aniFilSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		aniFilSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		aniFilSampleDesc.MipLODBias = 0;
		aniFilSampleDesc.MaxAnisotropy = D3D11_DEFAULT_MAX_ANISOTROPY;
		aniFilSampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		aniFilSampleDesc.MinLOD = -FLT_MAX;
		aniFilSampleDesc.MaxLOD = FLT_MAX;

		result = device->CreateSamplerState(&aniFilSampleDesc, &mySamplerStates[E_SAMPLERSTATE_ANISTROPIC_WRAP]);
		assert(SUCCEEDED(result));

		mySamplerStates[E_SAMPLERSTATE_TRILINEAR] = nullptr;

		SetSamplerState(ESamplerState::E_SAMPLERSTATE_TRILINEAR, 0);
		SetSamplerState(ESamplerState::E_SAMPLERSTATE_TRILINEAR_WRAP, 1);
		SetSamplerState(ESamplerState::E_SAMPLERSTATE_POINT, 2);
		SetSamplerState(ESamplerState::E_SAMPLERSTATE_ANISTROPIC_WRAP, 3);

		CreateTextures();
		CContentLoader* const& content = CEngine::GetInstance()->GetContentLoader();
		myBackBuffer = content->Load(backBufferTexture);
		Postmaster::GetInstance().Subscribe(this, eMessage::eUpdateResolution);
		Postmaster::GetInstance().Subscribe(this, eMessage::eCullBack);
		Postmaster::GetInstance().Subscribe(this, eMessage::eCullFront);
		Postmaster::GetInstance().Subscribe(this, eMessage::eCullNone);

		Vector2ui sres = DX11::GetResolution();
		uint2 gres = Singleton<GlobalSettings>().gameplayResolution;
		myPostProcessingData.screenResolution.x = sres.x;
		myPostProcessingData.screenResolution.y = sres.y;
		myPostProcessingData.gameResolution.x = gres.x;
		myPostProcessingData.gameResolution.y = gres.y;

		SetRasterizerState(ERasterizerState::E_RASTERIZERSTATE_CULLNONE);

		printe("render manager inited\n");
		return true;
	}

	// TODO: remove trash and clean up vvvv
	void CRenderManager::RenderFrame()
	{
		//for (int i = int(eInputEvent::ToggleAllPasses); i < int(eInputEvent::ToggleAllPasses) + 8; i++)
		//{
		//	if (Input::GetInputPressed((eInputEvent)i))
		//	{
		//		myRenderLayer = i - int(eInputEvent::ToggleAllPasses);
		//		break;
		//	}
		//}

		//if (Input::GetInputPressed(eInputEvent::ToggleMaterialPass))
		//{
		//	myRenderLayer++;
		//	if (myRenderLayer > 7)
		//	{
		//		myRenderLayer = 0;
		//	}
		//}

		myBackBuffer.ClearTexture({ 1, 1, 1, 1 });
		myFullscreen.ClearTexture();
		myFullscreenCopy.ClearTexture();
		myIntermediateDepth.ClearDepth();
		myScaledBackBuffer.ClearTexture();
		myGameWindow.ClearTexture();

		myPostProcessingData.volumetricIndex = 0;

		float dt = CEngine::GetInstance()->GetDeltaTime();
		myPostProcessingData.elapsedTime += dt;
		myPostProcessingData.TimePI += dt;
		myPostProcessingData.deltaTime = dt;
		if (myPostProcessingData.TimePI >= PI * 2)
		{
			myPostProcessingData.TimePI -= PI * 2;
		}

		CScene* scene = CEngine::GetInstance()->GetActiveScene();
		GameManager& gm = CEngine::GetInstance()->GetGameManager();
		CameraComponent* mainCam = nullptr;

		// TODO: Implement main or active cam in scene
		for (auto& [entity, component] : gm.GetComponentMap<EditorController>().map)
		{
			mainCam = gm.GetEntity(entity).GetComponent<CameraComponent>();
		}

		if (!mainCam)
		{
			auto& cameraMap = gm.GetComponentMap<CameraComponent>();

			// picks first best available camera, might fix later
			if (!cameraMap.map.empty())
			{
				mainCam = cameraMap.map.begin()->second;
			}
		}

		D3D11_MAPPED_SUBRESOURCE bufferData = { 0 };
		ID3D11DeviceContext* context = CEngine::GetInstance()->GetDXDeviceContext();
		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		HRESULT result = context->Map(myPostProcessingBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			return;
		}

		memcpy(bufferData.pData, &myPostProcessingData, sizeof(PostProcessingData));
		context->Unmap(myPostProcessingBuffer, 0);
		context->VSSetConstantBuffers(3, 1, &myPostProcessingBuffer);
		context->PSSetConstantBuffers(3, 1, &myPostProcessingBuffer);

		myIntermediateDepth.ClearDepth();

		if (mainCam)
		{
			SetBlendState(E_BLENDSTATE_ALPHABLEND);
			myScaledBackBuffer.SetAsActiveTarget();
			myForwardRenderer.RenderSprites(mainCam, scene->GetSprites());
			myForwardRenderer.RenderSprites(mainCam, scene->GetSSSprites(), true);
			SetBlendState(E_BLENDSTATE_DISABLE);
		}

		myFullscreen.SetAsActiveTarget();
		myScaledBackBuffer.SetAsResourceOnSlot(0);
		myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Copy);

		// line drawer
		{
			if (Input::GetInputPressed(eInputEvent::ToggleDrawLine))
			{
				myDrawLines = !myDrawLines;
				CLineDrawer::SetEnabled(myDrawLines);
			}

			//if (myLinesUseDepth)
			//{
			//	myScaledBackBuffer.SetAsActiveTarget(1, &myIntermediateDepth);
			//}
			//else
			//{
				//myScaledBackBuffer.SetAsActiveTarget();
			//}
			if (mainCam)
			{
				SetBlendState(E_BLENDSTATE_ALPHABLEND);
				CLineDrawer::Render();
				SetBlendState(E_BLENDSTATE_DISABLE);
			}
		}

		myGameWindow.SetAsActiveTarget();
		myFullscreen.SetAsResourceOnSlot(0);
		myFullscreenRenderer.Render(CFullscreenRenderer::EShader_ToRawColor);
		myFullscreen.SetAsActiveTarget();

		CLineDrawer::Clear();
	}

	void CRenderManager::DrawCursor()
	{
		CommonUtilities::RefillVector<SE::CSprite*> sprites;
		auto pos = Input::GetMousePos();
		//myCursor->SetPosition(Singleton<GlobalSettings>().gameWindowRect.xy);
		myCursor->SetPosition(pos);
		if (!Input::GetLockedCursorState())
			sprites.push_back(myCursor);

		SetBlendState(E_BLENDSTATE_ALPHABLEND);
		myScaledBackBuffer.ClearTexture();
		myScaledBackBuffer.SetAsActiveTarget();
		myForwardRenderer.RenderSprites(sprites);
		SetBlendState(E_BLENDSTATE_DISABLE);

		myFullscreenCopy.SetAsActiveTarget();
		myFullscreen.SetAsResourceOnSlot(0);
		myScaledBackBuffer.SetAsResourceOnSlot(1);
		myFullscreenRenderer.Render(CFullscreenRenderer::EShader_AlphaBlend);

		myBackBuffer.SetAsActiveTarget();
		myFullscreenCopy.SetAsResourceOnSlot(0);
		myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Copy);
	}

	void CRenderManager::RecieveMessage(eMessage aMsg)
	{
		switch (aMsg)
		{
		case eMessage::eUpdateResolution:
			CreateTextures();
			break;
		case eMessage::eCullBack:
			SetRasterizerState(ERasterizerState::E_RASTERIZERSTATE_DEFAULT);
			break;
		case eMessage::eCullFront:
			SetRasterizerState(ERasterizerState::E_RASTERIZERSTATE_CULLFRONT);
			break;
		case eMessage::eCullNone:
			SetRasterizerState(ERasterizerState::E_RASTERIZERSTATE_CULLNONE);
		default:
			break;
		}
	}

	void CRenderManager::CreateTextures()
	{
		Vector2ui res = Singleton<GlobalSettings>().gameplayResolution;////*{ 1920, 1080 };*/DX11::GetResolution();
		CContentLoader* const& content = CEngine::GetInstance()->GetContentLoader();

		myScaledBackBuffer.Release();
		myIntermediateDepth.Release();
		myFullscreenCopy.Release();
		myFullscreen.Release();
		myGameWindow.Release();

		//DXGI_FORMAT hdrFormat = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
		//DXGI_FORMAT normalFormat = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

		myFullscreen = content->Load(DX11::GetResolution(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		myFullscreenCopy = content->Load(DX11::GetResolution(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		myGameWindow = content->Load(DX11::GetResolution(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		myScaledBackBuffer = content->Load(res, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		myIntermediateDepth = content->GetTextureFactory().CreateFullscreenDepth(res, DXGI_FORMAT_R32_TYPELESS);

		Singleton<GlobalSettings>().gameViewTexture = &myGameWindow;
	}

	void CRenderManager::SetBlendState(EBlendState aBlendState)
	{
		CEngine::GetInstance()->GetDXDeviceContext()->OMSetBlendState(myBlendStates[aBlendState], 0, 0xffffffff);
	}

	void CRenderManager::SetDepthStencilState(EDepthStencilState aStencilState)
	{
		CEngine::GetInstance()->GetDXDeviceContext()->OMSetDepthStencilState(myDepthStencilStates[aStencilState], 1);
	}

	void CRenderManager::SetRasterizerState(ERasterizerState aRasterizerState)
	{
		CEngine::GetInstance()->GetDXDeviceContext()->RSSetState(myRasterizerStates[aRasterizerState]);
	}

	void CRenderManager::SetSamplerState(ESamplerState aSamplerState, int aSlot)
	{
		CEngine::GetInstance()->GetDXDeviceContext()->PSSetSamplers(aSlot, 1, &mySamplerStates[aSamplerState]);
	}
}
