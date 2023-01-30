#pragma once
#include <string>
#include <CommonUtilities\Container\HashMap.h>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11GeometryShader;
struct ID3D11InputLayout;
struct D3D11_INPUT_ELEMENT_DESC;
namespace SE
{
    namespace Helper
    {
        namespace ShaderHelper
        {
            bool CreateVertexShader(ID3D11VertexShader** aVertexShader, const std::string& aFilePath);
            bool CreateVertexShader(ID3D11VertexShader** aVertexShader, const std::string& aFilePath, std::string* const& anOutData);
            bool CreatePixelShader(ID3D11PixelShader** aPixelShader, const std::string& aFilePath);
            bool CreatePixelShader(ID3D11PixelShader** aPixelShader, const std::string& aFilePath, std::string* const& anOutData);
            bool CreateGeometryShader(ID3D11GeometryShader** aGeometryShader, const std::string& aFilePath);
            bool CreateGeometryShader(ID3D11GeometryShader** aGeometryShader, const std::string& aFilePath, std::string* const& anOutData);

            bool CreateInputLayout(ID3D11InputLayout** anInputLayout, D3D11_INPUT_ELEMENT_DESC* aLayout, uint aNumberOfElements, const std::string& someInData);
        }
    }
}
