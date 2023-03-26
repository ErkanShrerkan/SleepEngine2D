#pragma once
#pragma comment(lib, "d3d11.lib")
#include <dxgiformat.h>
#include <string>
#include "FullscreenTexture.h"
#include "GBuffer.h"
#include <unordered_map>
#include <memory>
#include <mutex>
#include <queue>
#include <atomic>

struct ID3D11Texture2D;
namespace SE
{
    class CTexture;
    class CTextureResource;
    struct STextureResourcePtr;
    class CTextureFactory
    {
    public:
        CTextureFactory();
        ~CTextureFactory();
        CFullscreenTexture CreateFullscreenTexture(const Vector2ui& aSize, DXGI_FORMAT aFormat);
        CFullscreenTexture CreateFullscreenDepth(const Vector2ui& aSize, DXGI_FORMAT aFormat);
        CFullscreenTexture CreateShadowMap(const Vector2ui& aSize);
        CFullscreenTexture CreateFullscreenTexture(ID3D11Texture2D* const& aTexture);
        CGBuffer CreateGBuffer(const Vector2ui& aSize);

        void Update();
        sptr(CTexture) LoadTexture(const std::string& aPath);
        
    private:
        struct QueuedResource
        {
            sptr(STextureResourcePtr) resourcePtr;
            std::string path;
        };

        void LoadResource(QueuedResource& aQueuedResource);

    private:

        std::unordered_map<std::string, wptr(CTextureResource)> myResourcePool;
        std::queue<QueuedResource> myResourceLoadQueue;

        std::mutex myMutex;
        std::atomic_bool myLoading = false;
    };
}
