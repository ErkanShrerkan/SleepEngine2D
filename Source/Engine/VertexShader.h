#pragma once
#include <d3d11.h>

namespace SE
{

	class VertexShader
	{
	public:
		VertexShader(const std::string& aVertexShaderPath);
		~VertexShader();

		ID3D11VertexShader* Raw();

	private:
		void Reload();
		void Release();

	private:
		std::string myPath;
		ID3D11VertexShader* myVS = nullptr;
	};

}
