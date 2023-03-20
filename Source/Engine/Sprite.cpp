#include "pch.h"
#include "Sprite.h"
#include "Texture.h"
#include <Engine/DX11.h>
#include "Engine.h"
#include "Scene.h"
#include <Engine\ContentLoader.h>
#include <Game\Globals.h>

namespace SE
{
    void CSprite::SetPosition(const Vector2f& aPosition)
    {
        myPosition.x = aPosition.x;
        myPosition.y = aPosition.y;
    }
    void CSprite::SetSize(const Vector2f& aSize)
    {
        mySize = aSize;
    }
    void CSprite::SetSizeRelativeToScreen(const Vector2f& aSize)
    {
        SetSize(aSize);
    }
    void CSprite::SetSizeRelativeToImage(const Vector2f& aSize)
    {
        mySize = GetNormalizedImageSize();
        SetSize(mySize * aSize);
    }
    void CSprite::SetSizeRelativeToAnotherImage(const float2& aSize, CSprite& aSprite)
    {
        mySize = aSprite.GetNormalizedImageSize();
        SetSize(mySize * aSize);
    }
    void CSprite::SetSizeRelativeToAnotherImage(const float2& aSize, CSprite* aSprite)
    {
        mySize = aSprite->GetNormalizedImageSize();
        SetSize(mySize * aSize);
    }
    void CSprite::SetColor(const Vector4f& aColor)
    {
        myColor = aColor;
    }
    void CSprite::SetPivot(const Vector2f& aPivot)
    {
        myPivot = aPivot;
    }
    void CSprite::SetRotation(const float& aRotation)
    {
        float rot = aRotation * (1.f / 360.f);
        rot -= (int)rot;
        rot *= 360.f;
        myRotation = rot * (1.f / 180.f * 3.14159265f);
    }
    void CSprite::SetMask(const char* aFilePath)
    {
        myMaskTexture = Singleton<CTextureFactory>().LoadTexture(aFilePath);
    }
    void CSprite::SetRect(const float4 aRect)
    {
        myRect = aRect;
    }
    void CSprite::Render()
    {
        //CEngine::GetInstance()->GetActiveScene()->AddInstance(this);
    }
    void CSprite::SetShaderData(float someData)
    {
        myData = someData;
    }
    Vector2f& CSprite::GetPosition()
    {
        return myPosition;
    }
    Vector2f CSprite::GetTopLeftPosition()
    {
        float2 size = GetNormalizedImageSize();
        float2 pivot = GetPivot();
        return myPosition - float2( mySize.x * myPivot.x, mySize.y * myPivot.y );
    }
    Vector2f& CSprite::GetSize()
    {
        return mySize;
    }
    Vector2f& CSprite::GetPivot()
    {
        return myPivot;
    }
    Vector4f& CSprite::GetColor()
    {
        return myColor;
    }
    Vector2f CSprite::GetImageSize()
    {
        WaitIfTextureIsNotLoaded();
        return Vector2f(myTexture->GetWidth(), myTexture->GetHeight());
    }
    Vector2f CSprite::GetNormalizedImageSize()
    {
        float x = GetImageSize().x / 100;
        float y = GetImageSize().y / 100;
        return { x, y };
    }
    float4 CSprite::GetRect()
    {
        return myRect;
    }
    float CSprite::GetRotation()
    {
        return myRotation * (180.f / 3.14159265f);
    }

    void CSprite::Release()
    {
        SE::CEngine::GetInstance()->GetContentLoader()->GetSpriteFactory().ReleaseSprite(this);
    }
    void CSprite::WaitIfTextureIsNotLoaded()
    {
        if (TextureResourceHasLoaded())
            return;

        bool loaded = false;
        while (!loaded)
        {
            Sleep(1);
            if (TextureResourceHasLoaded())
            {
                loaded = true;
            }
        }
    }
    bool CSprite::TextureResourceHasLoaded()
    {
        return true;//myTexture->GetShaderResourceView();
    }
}
