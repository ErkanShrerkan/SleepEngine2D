#pragma once

struct ID3D11VertexShader;
struct ID3D11InputLayout;
namespace SE
{
	class VertexShader
	{
	public:
		VertexShader(const std::string& aVertexShaderPath);
		VertexShader(){}
		~VertexShader();

		void Set(const std::string& aVertexShaderPath);

		ID3D11VertexShader* Raw();

	private:
		void Reload();
		void Release();

	private:
		std::string myPath;
		ID3D11VertexShader* myVS = nullptr;
		ID3D11InputLayout* myInputLayout = nullptr;
	};

}
