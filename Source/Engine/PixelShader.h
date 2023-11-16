#pragma once
#include <d3d11.h>

namespace SE
{

	class PixelShader
	{
	public:
		PixelShader(const std::string& aPixelShaderPath);
		PixelShader(){}
		PixelShader(const PixelShader& aPS);
		~PixelShader();

		void Set(const std::string& aPixelShaderPath);

		ID3D11PixelShader* Raw();
		void Release();

	private:
		void Reload();

	private:
		std::string myPath;
		ID3D11PixelShader* myPS = nullptr;
	};

}
