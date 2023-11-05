#include "pch.h"
#include "ShaderHelper.h"
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include "Engine.h"
#include <fstream>

#define SE_HELPER_SHADER_FILE_EXTENSION ".cso"

namespace SE
{
    namespace Helper
    {
        namespace ShaderHelper
        {
            class VertexShaderBank
            {
            public:
                bool GetVertexShader(ID3D11VertexShader** aVertexShader, const std::string& aFilePath, std::string& aBytecode)
                {
                    auto it = myVertexShaders.find(aFilePath);
                    if (it != myVertexShaders.end())
                    {
                        *aVertexShader = it->second;
                        return true;
                    }

                    return CreateVertexShader(aVertexShader, aFilePath, &aBytecode);
                }
            private:
                bool CreateVertexShader(ID3D11VertexShader** aVertexShader, const std::string& aFilePath, std::string* const& anOutData)
                {
                    HRESULT result;
                    ID3D11Device* const& device = CEngine::GetInstance()->GetDXDevice();

                    std::ifstream vsFile;
                    vsFile.open(aFilePath + SE_HELPER_SHADER_FILE_EXTENSION, std::ios::binary);
                    *anOutData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
                    result = device->CreateVertexShader((*anOutData).data(), (*anOutData).size(), nullptr, aVertexShader);
                    vsFile.close();
                    if (FAILED(result))
                    {
                        /* Error Message */
                        perr("Error creating Vertex Shader \"%s\"", (aFilePath + SE_HELPER_SHADER_FILE_EXTENSION).c_str());
                    }

                    myVertexShaders[aFilePath] = *aVertexShader;

                    return *aVertexShader != nullptr;
                }
            private:
                std::unordered_map<std::string, ID3D11VertexShader*> myVertexShaders;
            };

            struct PixelShaderBank
            {
            public:
                bool GetPixelShader(ID3D11PixelShader** aPixelShader, const std::string& aFilePath)
                {
                    auto it = myPixelShaders.find(aFilePath);
                    if (it != myPixelShaders.end())
                    {
                        *aPixelShader = it->second;
                        return true;
                    }

                    std::string psData;
                    return CreatePixelShader(aPixelShader, aFilePath, &psData);
                }
            private:
                bool CreatePixelShader(ID3D11PixelShader** aPixelShader, const std::string& aFilePath, std::string* const& anOutData)
                {
                    HRESULT result;
                    ID3D11Device* const& device = CEngine::GetInstance()->GetDXDevice();

                    std::ifstream psFile;
                    psFile.open(aFilePath + SE_HELPER_SHADER_FILE_EXTENSION, std::ios::binary);
                    *anOutData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
                    result = device->CreatePixelShader((*anOutData).data(), (*anOutData).size(), nullptr, aPixelShader);
                    psFile.close();
                    if (FAILED(result))
                    {
                        /* Error Message */
                        perr("Error creating Pixel Shader \"%s\"", (aFilePath + SE_HELPER_SHADER_FILE_EXTENSION).c_str());
                    }

                    myPixelShaders[aFilePath] = *aPixelShader;

                    return *aPixelShader != nullptr;
                }
            private:
                std::unordered_map<std::string, ID3D11PixelShader*> myPixelShaders;
            };

            bool CreateVertexShader(ID3D11VertexShader** aVertexShader, const std::string& aFilePath, std::string& aBytecode)
            {
                return Singleton<VertexShaderBank>().GetVertexShader(aVertexShader, aFilePath, aBytecode);
            }

            bool CreateVertexShader(ID3D11VertexShader** aVertexShader, const std::string& aFilePath)
            {
                std::string vsData;
                return Singleton<VertexShaderBank>().GetVertexShader(aVertexShader, aFilePath, vsData);
            }

            bool CreateVertexShader(ID3D11VertexShader** aVertexShader, const std::string& aFilePath, std::string* const& anOutData)
            {
                HRESULT result;
                ID3D11Device* const& device = CEngine::GetInstance()->GetDXDevice();

                std::ifstream vsFile;
                vsFile.open(aFilePath + SE_HELPER_SHADER_FILE_EXTENSION, std::ios::binary);
                *anOutData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
                result = device->CreateVertexShader((*anOutData).data(), (*anOutData).size(), nullptr, aVertexShader);
                vsFile.close();
                if (FAILED(result))
                {
                    /* Error Message */
                    perr("Error creating Vertex Shader \"%s\"", (aFilePath + SE_HELPER_SHADER_FILE_EXTENSION).c_str());
                }

                return *aVertexShader != nullptr;
            }

            bool CreatePixelShader(ID3D11PixelShader** aPixelShader, const std::string& aFilePath)
            {
                return Singleton<PixelShaderBank>().GetPixelShader(aPixelShader, aFilePath);
            }

            bool CreatePixelShader(ID3D11PixelShader** aPixelShader, const std::string& aFilePath, std::string* const& anOutData)
            {
                HRESULT result;
                ID3D11Device* const& device = CEngine::GetInstance()->GetDXDevice();

                std::ifstream psFile;
                psFile.open(aFilePath + SE_HELPER_SHADER_FILE_EXTENSION, std::ios::binary);
                *anOutData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
                result = device->CreatePixelShader((*anOutData).data(), (*anOutData).size(), nullptr, aPixelShader);
                psFile.close();
                if (FAILED(result))
                {
                    /* Error Message */
                    perr("Error creating Pixel Shader \"%s\"", (aFilePath + SE_HELPER_SHADER_FILE_EXTENSION).c_str());
                }

                return *aPixelShader != nullptr;
            }

            bool CreateGeometryShader(ID3D11GeometryShader** aGeometryShader, const std::string& aFilePath)
            {
                std::string gsData;
                return CreateGeometryShader(aGeometryShader, aFilePath, &gsData);
            }
            bool CreateGeometryShader(ID3D11GeometryShader** aGeometryShader, const std::string& aFilePath, std::string* const& anOutData)
            {
                HRESULT result;
                ID3D11Device* const& device = CEngine::GetInstance()->GetDXDevice();

                std::ifstream gsFile;
                gsFile.open(aFilePath + SE_HELPER_SHADER_FILE_EXTENSION, std::ios::binary);
                *anOutData = { std::istreambuf_iterator<char>(gsFile), std::istreambuf_iterator<char>() };
                result = device->CreateGeometryShader((*anOutData).data(), (*anOutData).size(), nullptr, aGeometryShader);
                gsFile.close();
                if (FAILED(result))
                {
                    /* Error Message */
                    perr("Error creating Geometry Shader \"%s\"", (aFilePath + SE_HELPER_SHADER_FILE_EXTENSION).c_str());
                }

                return *aGeometryShader != nullptr;
            }
            bool CreateInputLayout(ID3D11InputLayout** anInputLayout, D3D11_INPUT_ELEMENT_DESC* aLayout, uint aNumberOfElements, const std::string& someInData)
            {
                HRESULT result;
                ID3D11Device* const& device = CEngine::GetInstance()->GetDXDevice();

                result = device->CreateInputLayout(aLayout, aNumberOfElements, someInData.data(), someInData.size(), anInputLayout);
                if (FAILED(result))
                {
                    /* Error Message */
                    perr("Error creating Input Layout");
                }
                return *anInputLayout != nullptr;
            }
        }
    }
}

