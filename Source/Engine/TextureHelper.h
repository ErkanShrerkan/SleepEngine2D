#pragma once
#include <string_view>
#include <unordered_map>
#include <CommonUtilities\Container\HashMap.h>
#include <set>
#include <mutex>
#include <memory>

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

                //void RegisterTextureSet(const std::string& aTextureSet) 
                //{ 
                //    //myMutex.lock();
                //    textureSets.insert(aTextureSet); 
                //    //myMutex.unlock();
                //}
                bool GetResource(ID3D11ShaderResourceView** aShaderResourceView, const std::string& aFilePath);
                void Release();

            private:
                bool CreateResource(ID3D11ShaderResourceView** aShaderResourceView, const std::string& aFilePath);
                //std::set<std::string>& GetTextureSets() { return textureSets; }

            private:
               // std::unordered_map<std::string, ID3D11ShaderResourceView*> resources;
                //std::set<std::string> textureSets;
                std::mutex myMutex;
            };
        }
    }
}
