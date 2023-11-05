#include "pch.h"
#include "VertexShader.h"
#include "ShaderHelper.h"

namespace SE
{

	VertexShader::VertexShader(const std::string& aVertexShaderPath)
	{
		myPath = aVertexShaderPath;
		Reload();
	}

	VertexShader::~VertexShader()
	{
		Release();
	}

	ID3D11VertexShader* VertexShader::Raw()
	{
		return myVS;
	}

	void VertexShader::Reload()
	{
		Release();
		SE::Helper::ShaderHelper::CreateVertexShader(&myVS, myPath);
	}

	void VertexShader::Release()
	{
		if (!myVS)
		{
			myVS->Release();
		}

		myVS = nullptr;
	}

}