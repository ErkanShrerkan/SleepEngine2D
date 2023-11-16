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
		VertexShader(const VertexShader& aVS);
		~VertexShader();

		void Set(const std::string& aVertexShaderPath);

		ID3D11VertexShader* Raw();
		ID3D11InputLayout* const& GetInputLayout();

		void Release();

	private:
		void Reload();

	private:
		std::string myPath;
		ID3D11VertexShader* myVS = nullptr;
		ID3D11InputLayout* myInputLayout = nullptr;
	};

}
