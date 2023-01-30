#include "pch.h"
#include "ContentLoader.h"

namespace SE
{
    CFullscreenTexture CContentLoader::Load(const Vector2ui& aSize, DXGI_FORMAT aFormat)
    {
        return myTextureFactory.CreateFullscreenTexture(aSize, aFormat);
    }
    CFullscreenTexture CContentLoader::Load(ID3D11Texture2D* aTexture)
    {
        return myTextureFactory.CreateFullscreenTexture(aTexture);
    }
    SE::CLightFactory& CContentLoader::GetLightFactory()
    {
        return myLightFactory;
    }
    SE::CTextureFactory& CContentLoader::GetTextureFactory()
    {
        return myTextureFactory;
    }
    SE::CSpriteFactory& CContentLoader::GetSpriteFactory()
    {
        return mySpriteFactory;
    }
}
