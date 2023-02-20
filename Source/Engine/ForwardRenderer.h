#pragma once
#include "ShaderDefines.h"
#include "AutoReleaser.h"
#include <array>
#include <CommonUtilities\Container\RefillVector.h>

struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
class CameraComponent;
class Sprite;

namespace SE
{
	class CSprite;
	class CDirectX11Framework;
	class CForwardRenderer
	{
	public:
		CForwardRenderer();
		~CForwardRenderer();

		bool Init(CDirectX11Framework* aFramework);
		void RenderSprites(CameraComponent* aCamera, CommonUtilities::RefillVector<Sprite*>& someSprites, bool aIsScreenSpace = false);
		void RenderSprites(CommonUtilities::RefillVector<CSprite*>& someSprites);
	private:
		struct SFrameBufferData
		{
			float4x4 myToCamera;
			float4x4 myCameraTransform;
			float4x4 myToProjection;
			float4 myCameraPosition;
		};

		struct SSpriteBufferData
		{
			float4x4 myTransform;
			float4 myColor;
			float4 myRect;
			float2 mySize;
			float2 myPosOffset;
			float2 myPivot;
			float myRotation;
			float myData;
		};

		SFrameBufferData myFrameBufferData;
		SSpriteBufferData mySpriteBufferData;

		ID3D11DeviceContext* myContext;
		AutoReleaser<ID3D11Buffer> myFrameBuffer;
		AutoReleaser<ID3D11Buffer> myObjectBuffer;
		AutoReleaser<ID3D11Buffer> mySpriteBuffer;

		ID3D11PixelShader* myFisheyePixelShader;
		ID3D11PixelShader* myCircularFillPixelShader;
		ID3D11PixelShader* mySpritePixelShader;
		ID3D11VertexShader* mySpriteVertexShader;
		ID3D11VertexShader* myWorldSpriteVertexShader;
	};
}
