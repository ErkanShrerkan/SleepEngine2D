#pragma once
#include <memory>
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct D3D11_VIEWPORT;
struct ID3D11DeviceContext;
namespace SE
{
    class CFullscreenTexture
    {
        friend class CTextureFactory;
        friend class CGBuffer;
    public:
        //CFullscreenTexture();
        ~CFullscreenTexture();

        void Release();
        void ClearTexture(const Vector4f& aClearColor = { 0.f, 0.f, 0.f, 0.f });
        void ClearDepth(float aClearDepthValue = 1.0f, uint aClearStencilValue = 0u);
        void SetAsActiveTarget(int anIndex = 1, CFullscreenTexture* aDepth = nullptr);
        void SetAsActiveDepth();
        void SetAsResourceOnSlot(uint aSlot);
        void SetDeviceContext(ID3D11DeviceContext* aContext);
        ID3D11Texture2D* GetTexture() { return myTexture; }
        ID3D11ShaderResourceView* GetSRV() { return myShaderResourceView; }

    private:
        
        union
        {
            ID3D11RenderTargetView* myRenderTargetView = nullptr;
            ID3D11DepthStencilView* myDepth;
        };

        ID3D11Texture2D* myTexture = nullptr;
        ID3D11ShaderResourceView* myShaderResourceView = nullptr;
        std::shared_ptr<D3D11_VIEWPORT> myViewport = nullptr;

        // Not Owned by this
        ID3D11DeviceContext* myContext = nullptr;
    };
}
