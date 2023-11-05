#pragma once
#include "ForwardRenderer.h"
#include "FullscreenRenderer.h"
#include "FullscreenTexture.h"
#include "GBuffer.h"
#include <Game\Observer.h>
#include <math.h>

struct CDirectX11Framework;
class Sprite;
class Entity;
namespace SE
{
	class CRenderManager : public Observer
	{
	public:
		struct PostProcessingData;

	public:
		~CRenderManager();

		bool Restart();
		bool Init(CDirectX11Framework* aFramework);
		void RenderFrame();
		void DrawCursor();
		void Update();
		void RecieveMessage(eMessage aMsg) override;
		PostProcessingData& GetPPD() { return myPostProcessingData; }

	public:
		enum EBlendState
		{
			E_BLENDSTATE_DISABLE,
			E_BLENDSTATE_ALPHABLEND,
			E_BLENDSTATE_ADDITIVE,

			E_BLENDSTATE_COUNT
		};

		enum EDepthStencilState
		{
			E_DEPTHSTENCILSTATE_DEFAULT,
			E_DEPTHSTENCILSTATE_READONLY,

			E_DEPTHSTENCILSTATE_COUNT
		};

		enum ERasterizerState
		{
			E_RASTERIZERSTATE_DEFAULT,
			E_RASTERIZERSTATE_CULLFRONT,
			E_RASTERIZERSTATE_CULLNONE,
			E_RASTERIZERSTATE_WIREFRAME,

			E_RASTERIZERSTATE_COUNT
		};

		enum ESamplerState
		{
			E_SAMPLERSTATE_TRILINEAR,
			E_SAMPLERSTATE_TRILINEAR_WRAP,
			E_SAMPLERSTATE_POINT,
			E_SAMPLERSTATE_ANISTROPIC_WRAP,

			E_SAMPLERSTATE_COUNT
		};

		struct PostProcessingData
		{
			float3 shallowWaterColor;
			float waterLevel;
			float3 deepWaterColor;
			float refractionIndex;
			float3 waterBorderColor;
			float refractionStrength;
			uint2 screenResolution;
			uint2 gameResolution;
			float borderThreshhold;
			float TimePI;
			float deltaTime;
			float fogDensity;
			float3 fogColor;
			float fogMult;
			float fogOffset;
			float godRaySampleDistance;
			float godRayFalloff;
			float godRayScatterProbability;
			float alphaThreshold;
			int config;
			float mainCamNearPlane;
			float elapsedTime;
			float emissiveMult;
			float bloomMult;
			int volumetricIndex;
			float y;
		};

		struct FrameBufferData
		{
			Matrix4x4f cameraTransform;
			Matrix4x4f toCamera;
			Matrix4x4f toProjection;
			Vector4f cameraPosition;
		};

	private:
		void ActuallyRestart();
		void CreateTextures();
		void BindSamplerStates();
		void SetBlendState(EBlendState aBlendState);
		void SetDepthStencilState(EDepthStencilState aStencilState);
		void SetRasterizerState(ERasterizerState aRasterizerState);
		void SetSamplerState(ESamplerState aSamplerState, int aSlot = 0);

	private:
		CForwardRenderer myForwardRenderer;
		CFullscreenRenderer myFullscreenRenderer;

		CFullscreenTexture myBackBuffer;
		CFullscreenTexture myFullscreen;
		CFullscreenTexture myFullscreenCopy;
		CFullscreenTexture myScaledBackBuffer;
		CFullscreenTexture myGameWindow;
		CFullscreenTexture myRGBTexture;

		CFullscreenTexture myIntermediateDepth;

		std::array<ID3D11BlendState*, E_BLENDSTATE_COUNT> myBlendStates;
		std::array<ID3D11DepthStencilState*, E_DEPTHSTENCILSTATE_COUNT> myDepthStencilStates;
		std::array<ID3D11RasterizerState*, E_RASTERIZERSTATE_COUNT> myRasterizerStates;
		std::array<ID3D11SamplerState*, E_SAMPLERSTATE_COUNT> mySamplerStates;

		CSprite* myCursor;

		FrameBufferData myFrameBufferData;
		PostProcessingData myPostProcessingData;
		ID3D11Buffer* myPostProcessingBuffer = nullptr;
		CDirectX11Framework* myFrameWorkRef;

		int myRenderLayer = 0;
		bool myDrawLines = true;
		bool myRestartNextFrame = false;
	};
}
