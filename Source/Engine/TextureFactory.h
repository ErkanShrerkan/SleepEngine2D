#pragma once
#pragma comment(lib, "d3d11.lib")
#include <dxgiformat.h>
#include <string>
#include "FullscreenTexture.h"
#include "GBuffer.h"
#include <unordered_map>
#include <memory>

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
        sptr(CTexture) LoadTexture(const std::string& aPath);
        sptr(CTexture) CreateTexture(std::string& aPath);

    private:
        // TODO: Fix weak_ptr or shared_ptr or sumn 
        std::unordered_map<std::string, wptr(CTexture)> myPool;
    };
}
