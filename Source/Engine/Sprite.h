#pragma once
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11InputLayout;
namespace SE
{
	enum class SpriteShaderType
	{
		eNone,
		eFisheye,
		eCircularFill,

		eCount
	};

	class CTexture;
	class CSprite
	{
		friend class CForwardRenderer;
		friend class CSpriteFactory;
	public:
		/*

		 - Size
		 - Position
		 - Rotation
		 - Color

		 */
		void SetPosition(const Vector2f& aPosition);
		void SetSize(const Vector2f& aSize);
		void SetSizeRelativeToScreen(const Vector2f& aSize);
		void SetSizeRelativeToImage(const Vector2f& aSize);
		void SetSizeRelativeToAnotherImage(const float2& aSize, CSprite& aSprite);
		void SetSizeRelativeToAnotherImage(const float2& aSize, CSprite* aSprite);
		void SetColor(const Vector4f& aColor);
		void SetPivot(const Vector2f& aPivot);
		void SetRotation(const float& aRotation);
		void SetMask(const char* aFilePath);
		void SetRect(const float4 aRect);
		void Render();
		void SetShaderType(SpriteShaderType aType) { myShaderType = aType; }
		void SetShaderData(float someData);
		float GetShaderData() { return myData; }
		SpriteShaderType GetShaderType() { return myShaderType; }
		Vector2f& GetPosition();
		Vector2f GetTopLeftPosition();
		Vector2f& GetSize();
		Vector2f& GetPivot();
		Vector4f& GetColor();
		Vector2f GetImageSize();
		Vector2f GetNormalizedImageSize();
		float4 GetRect();
		float GetRotation();
		float GetRotationRadian() { return myRotation; };
		void Release();
		sptr(CTexture) GetTexture() { return myTexture; }

	private:
		void WaitIfTextureIsNotLoaded();
		bool TextureResourceHasLoaded();

	private:
		sptr(CTexture) myTexture;
		sptr(CTexture) myMaskTexture;
		Vector2f mySize;
		Vector2f myPosition;
		Vector4f myColor = { 1, 1, 1, 1 };
		float4 myRect;
		float2 myPivot;
		float myRotation;
		float myData;
		SpriteShaderType myShaderType = SpriteShaderType::eNone;

	private:
		struct Data
		{
			// From CSpriteFactory
			uint myStride;
			uint myOffset;
			ID3D11Buffer* myVertexBuffer;
			ID3D11Buffer* myIndexBuffer;
			ID3D11VertexShader* myVertexShader;
			ID3D11InputLayout* myInputLayout;
		};
	};
}
