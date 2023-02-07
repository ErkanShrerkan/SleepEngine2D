#include "pch.h"
#include "Sprite.h"
#include <Engine\Engine.h>
#include <Engine\ContentLoader.h>
#include <Engine\LineDrawer.h>
#include <Engine\Engine.h>
#include <Engine\Scene.h>
#include "Globals.h"

Sprite::Sprite(const std::string& aTexturePath, Entity*& myEntity)
	: Component::Component(myEntity), myTexture(aTexturePath)
{
	mySprite = SE::CEngine::GetInstance()->GetContentLoader()->GetSpriteFactory().GetSprite(aTexturePath);
}

void Sprite::Start()
{
	mySize = mySprite->GetSize();
	Expose(myTexture, "Texture");
	Expose(myRotation, "Rotation");
	Expose(myPosition, "Offset");
	Expose(mySize, "Size");
	Expose(myPivot, "Pivot");
	Expose(myColor, "Color");
	Expose(myRect, "Rect", ePickMode::Drag);
	Expose(myIsScreenSpace, "Is Screen Space");
	Expose(myRender, "Render");
}

void Sprite::Update()
{
	if (myRender)
	{
		Render();
	}
}

void Sprite::Reload()
{
	Release();
	mySprite = SE::CEngine::GetInstance()->GetContentLoader()->GetSpriteFactory().GetSprite(myTexture);
}

Sprite::~Sprite()
{
	Release();
}

void Sprite::DrawRect()
{
	float rot = mySprite->GetRotationRadian();
	float2 spriteSize = GetSize();
	float2 spritePos = GetPosition();
	float2 topLeft = GetTopLeftPosition();
	float2 topRight = { topLeft.x + spriteSize.x, topLeft.y };
	float2 bottomLeft = { topLeft.x, topLeft.y + spriteSize.y };
	float2 bottomRight = { topLeft.x + spriteSize.x, topLeft.y + spriteSize.y };
	float2 verts[] = { topLeft, topRight, bottomLeft, bottomRight };

	//float rads = rot / 180 * 3.14159265f;
	float c = cos(rot);
	float s = sin(rot);

	float aspect = (float)Singleton<GlobalSettings>().gameplayResolution.x / Singleton<GlobalSettings>().gameplayResolution.y;
	float aspect_rcp = 1.f / aspect;

	for (int i = 0; i < 4; i++)
	{
		float2& vert = verts[i];
		float2 pos = vert - spritePos;
		float2 newPos;
		{
			newPos.x = (pos.x * c) - (pos.y * s * aspect_rcp);
			newPos.y = (pos.x * s * aspect) + (pos.y * c);
		}
		vert = newPos + spritePos;
	}

	Debug::DrawLine2D(verts[0], verts[1]);
	Debug::DrawLine2D(verts[0], verts[2]);
	Debug::DrawLine2D(verts[2], verts[3]);
	Debug::DrawLine2D(verts[3], verts[1]);
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
	mySprite->SetRotation(aRotation);
	myRotation = aRotation;
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
	if (myIsScreenSpace)
	{
		mySprite->Render();
	}
	else
	{
		SE::CEngine::GetInstance()->GetActiveScene()->AddInstance(this);
	}
}

void Sprite::SetShaderData(float someData)
{
	mySprite->SetShaderData(someData);
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
