#include "pch.h"
#include "PixelShader.h"
#include "ShaderHelper.h"

namespace SE
{

	PixelShader::PixelShader(const std::string& aPixelShaderPath)
	{
		myPath = aPixelShaderPath;
		Reload();
	}

	PixelShader::PixelShader(const PixelShader& aPS)
	{
		myPath = aPS.myPath;
		myPS = aPS.myPS;
	}

	PixelShader::~PixelShader()
	{
		//Release();
	}

	void PixelShader::Set(const std::string& aPixelShaderPath)
	{
		myPath = aPixelShaderPath;
		Reload();
	}

	ID3D11PixelShader* PixelShader::Raw()
	{
		return myPS;
	}

	void PixelShader::Reload()
	{
		Release();
		SE::Helper::ShaderHelper::CreatePixelShader(&myPS, myPath);
	}

	void PixelShader::Release()
	{
		if (myPS)
		{
			myPS->Release();
		}

		myPS = nullptr;
	}

}