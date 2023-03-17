#include "pch.h"
#include "Sprite.h"
#include <Engine\Engine.h>
#include <Engine\ContentLoader.h>
#include <Engine\LineDrawer.h>
#include <Engine\Engine.h>
#include <Engine\Scene.h>
#include "Globals.h"
#include "Entity.h"
#include <Game\Transform.h>

Sprite::Sprite(const std::string& aTexturePath)
{
	myTexture.SetString(aTexturePath);
}

void Sprite::Start()
{
	mySprite = SE::CEngine::GetInstance()->GetContentLoader()->GetSpriteFactory().GetSprite(myTexture.GetString());
	mySize = mySprite->GetSize();
	Expose(myTexture, "Texture");
	Expose(myRotation, "Rotation", .1f, eBounds::Loop, { 0.f, 360.f });
	Expose(myPosition, "Offset", .25f);
	Expose(mySize, "Size", .25f);
	Expose(myPivot, "Pivot", .01f);
	Expose(myColor, "Color", .1f);
	Expose(myRect, "Rect", .01f, ePickMode::Drag);
	Expose(myIsScreenSpace, "Is Screen Space");
	Expose(myRender, "Render");
	Expose(myDrawRect, "Draw Rect");
}

void Sprite::Reload()
{
	Release();
	mySprite = SE::CEngine::GetInstance()->GetContentLoader()->GetSpriteFactory().GetSprite(myTexture.GetString());
}

Sprite::~Sprite()
{
	Release();
}

void Sprite::DrawRect()
{
	float2 size = GetSize();
	float4x4 scale;
	scale(1, 1) = size.x;
	scale(2, 2) = size.y;

	// get object transform in world space if it's a child
	float4x4 t = GameObject().GetComponent<Transform>()->GetTransform();
	float3 pos = t.GetPosition();
	float2 offset = GetPosition();
	pos += (t.GetRight() * offset.x) + (t.GetUp() * offset.y);
	t.SetRow(4, { 0, 0, 0, 1 });
	t = scale * t;
	t = t * float4x4::CreateRotationAroundZ(Math::DegreeToRadian(-GetRotation()));
	t.SetRow(4, { pos, 1 });

	float half = .5f;
	float4 verts[4] =
	{
		{ -half,  half, half, 1 },
		{  half,  half, half, 1 },
		{ -half, -half, half, 1 },
		{  half, -half, half, 1 },
	};

	float2 pivot = GetPivot();
	offset = float2(half - pivot.x, pivot.y - half);
	for (int i = 0; i < 4; i++)
	{
		float4& vert = verts[i];
		vert.xy += offset;
		vert = vert * t;
	}

	Debug::DrawLine2D(verts[0].xy, verts[1].xy, { 1, 1, 1, 1 }, true);
	Debug::DrawLine2D(verts[0].xy, verts[2].xy, { 1, 1, 1, 1 }, true);
	Debug::DrawLine2D(verts[2].xy, verts[3].xy, { 1, 1, 1, 1 }, true);
	Debug::DrawLine2D(verts[3].xy, verts[1].xy, { 1, 1, 1, 1 }, true);
}

void Sprite::SetPosition(const Vector2f& aPosition)
{
	myPosition = aPosition;

	uint2 res = Singleton<GlobalSettings>().gameplayResolution;
	float2 pixelSize = { 1.f / res.x, 1.f / res.y };
	float2 gridLockedPos = { aPosition.x / pixelSize.x, aPosition.y / pixelSize.y };
	gridLockedPos = { (float)(int)gridLockedPos.x / res.x, (float)(int)gridLockedPos.y / res.y };

	mySprite->SetPosition(gridLockedPos);
}

void Sprite::SetSize(const Vector2f& aSize)
{
	mySize = aSize;

	//uint2 res = Singleton<GlobalSettings>().gameplayResolution;
	//float2 pixelSize = { 1.f / res.x, 1.f / res.y };
	//float2 gridLockedSize = { aSize.x / pixelSize.x, aSize.y / pixelSize.y };
	//gridLockedSize = { (float)(int)gridLockedSize.x / res.x, (float)(int)gridLockedSize.y / res.y };

	mySprite->SetSize(aSize);
}

void Sprite::SetSizeRelativeToScreen(const Vector2f& aSize)
{
	SetSize(aSize);
}

void Sprite::SetSizeRelativeToImage(const Vector2f& aSize)
{
	mySize = GetNormalizedImageSize();
	SetSize(mySize * aSize);
}

void Sprite::SetSizeRelativeToAnotherImage(const float2& aSize, Sprite& aSprite)
{
	mySize = aSprite.GetNormalizedImageSize();
	SetSize(mySize * aSize);
}

void Sprite::SetSizeRelativeToAnotherImage(const float2& aSize, Sprite* aSprite)
{
	mySize = aSprite->GetNormalizedImageSize();
	SetSize(mySize * aSize);
}

void Sprite::SetWidthSizePreservedImageRatio(const float& aSize)
{
	float2 imgSize = GetImageSize();
	float ratio = imgSize.y / imgSize.x;
	SetSize({ aSize, aSize * ratio });
}

void Sprite::SetHeightSizePreservedImageRatio(const float& aSize)
{
	float2 imgSize = GetImageSize();
	float ratio = imgSize.x / imgSize.y;
	SetSize({ aSize * ratio, aSize });
}

void Sprite::SetColor(const Vector4f& aColor)
{
	mySprite->SetColor(aColor);
	myColor = aColor;
}

void Sprite::SetPivot(const Vector2f& aPivot)
{
	mySprite->SetPivot(aPivot);
	myPivot = aPivot;
}

void Sprite::SetRotation(const float& aRotation)
{
	myRotation = aRotation;
	myRotation += myRotation > 360.f ? -360.f : (myRotation < 0.f ? 360.f : 0);
	mySprite->SetRotation(aRotation);
}

void Sprite::SetMask(const std::string& aFilePath)
{
	mySprite->SetMask(aFilePath.c_str());
}

void Sprite::SetRect(const float4 aRect)
{
	mySprite->SetRect(aRect);
	myRect = aRect;
}

void Sprite::Render()
{
	if (myRender)
	{
		SE::CEngine::GetInstance()->GetActiveScene()->AddInstance(this);
	}
	if (myDrawRect)
	{
		DrawRect();
	}
}

void Sprite::SetShaderData(float someData)
{
	mySprite->SetShaderData(someData);
}

void Sprite::SetTexture(const std::string& aTexturePath)
{
	myTexture = aTexturePath;
}

std::string Sprite::GetTextureName()
{
	return myTexture.GetString();
}

Vector2f& Sprite::GetPosition()
{
	return myPosition;
}

Vector2f Sprite::GetTopLeftPosition()
{
	return mySprite->GetTopLeftPosition();
}

Vector2f& Sprite::GetSize()
{
	return mySize;
}

Vector2f& Sprite::GetPivot()
{
	return myPivot;
}

Vector4f& Sprite::GetColor()
{
	return myColor;
}

Vector2f Sprite::GetImageSize()
{
	return mySprite->GetImageSize();
}

Vector2f Sprite::GetNormalizedImageSize()
{
	return mySprite->GetNormalizedImageSize();
}

float4 Sprite::GetRect()
{
	return myRect;
}

float Sprite::GetRotation()
{
	return myRotation;
}

void Sprite::Release()
{
	mySprite->Release();
}
