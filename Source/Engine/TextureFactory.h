#pragma once
#pragma comment(lib, "d3d11.lib")
#include <dxgiformat.h>
#include <string>
#include "FullscreenTexture.h"
#include "GBuffer.h"
#include <unordered_map>

struct ID3D11Texture2D;
namespace SE
{
    class CTexture;
    class CTextureFactory
    {
    public:
        ~CTextureFactory();
        CFullscreenTexture CreateFullscreenTexture(const Vector2ui& aSize, DXGI_FORMAT aFormat);
        CFullscreenTexture CreateFullscreenDepth(const Vector2ui& aSize, DXGI_FORMAT aFormat);
        CFullscreenTexture CreateShadowMap(const Vector2ui& aSize);
        CFullscreenTexture CreateFullscreenTexture(ID3D11Texture2D* const& aTexture);
        CGBuffer CreateGBuffer(const Vector2ui& aSize);
        CTexture* LoadTexture(const std::string& aPath);
        CTexture* CreateTexture(std::string& aPath);

    private:

        std::unordered_map<std::string, CTexture*> myPool;
    };
}
