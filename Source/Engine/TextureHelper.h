#pragma once

struct ID3D11ShaderResourceView;
namespace SE
{
    namespace Helper
    {
        namespace TextureHelper
        {
            bool LoadShaderResourceView(ID3D11ShaderResourceView** aShaderResourceView, const std::string& aFilePath);

            struct TextureBank
            {
                friend class JsonManager;

                static TextureBank& GetInstance()
                {
                    static TextureBank instance;
                    return instance;
                }

                bool GetResource(ID3D11ShaderResourceView** aShaderResourceView, const std::string& aFilePath);

            private:
                bool CreateResource(ID3D11ShaderResourceView** aShaderResourceView, const std::string& aFilePath);

            private:
            };
        }
    }
}
