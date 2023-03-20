#pragma once
//#include <string_view>
//#include "AutoReleaser.h"

struct ID3D11ShaderResourceView;
namespace SE
{
    class CTexture
    {
    public:
        CTexture();
        CTexture(std::string& aFilePath);
        CTexture(const std::string& aFilePath);
        ~CTexture();
        ID3D11ShaderResourceView* const GetShaderResourceView() const noexcept;
        ID3D11ShaderResourceView* const* GetPointerToShaderResourceView() const;
        void SetShaderResourceView(ID3D11ShaderResourceView* const aShaderResourceView);
        // Calls AddRef on CTexture::myShaderResourceView, nullifies it, and returns its old value
        ID3D11ShaderResourceView* StealShaderResource();

        inline constexpr const float GetWidth() const noexcept
        {
            return myWidth;
        }
        inline constexpr const float GetHeight() const noexcept
        {
            return myHeight;
        }
        inline constexpr const float GetMipLevels() const noexcept
        {
            return myMipLevels;
        }

    private:
        void LoadResource();

    private:
        ID3D11ShaderResourceView* myShaderResourceView;
        float myWidth, myHeight, myMipLevels;
        unsigned myFormat;
        std::string myFilePath;
        bool myLoaded = false;
    };
}
