#include "pch.h"
#include "VertexShader.h"
#include "ShaderHelper.h"
#include "Engine.h"

#include <d3d11.h>

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

	void VertexShader::Set(const std::string& aVertexShaderPath)
	{
		myPath = aVertexShaderPath;
		Reload();
	}

	ID3D11VertexShader* VertexShader::Raw()
	{
		return myVS;
	}

	ID3D11InputLayout* VertexShader::GetInputLayout()
	{
		return myInputLayout;
	}

	void VertexShader::Reload()
	{
		Release();

		std::string vsData;

		SE::Helper::ShaderHelper::CreateVertexShader(&myVS, myPath, vsData);

		auto* device = CEngine::GetInstance()->GetDXDevice();

		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV",		  0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONEIDS",     0, DXGI_FORMAT_R32G32B32A32_UINT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

			{"RIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"UP", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 12, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"FORWARD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 24, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"POS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 36, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};

		HRESULT result = device->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &myInputLayout);

		if (FAILED(result))
		{
			assert("oh no");
		}
	}

	void VertexShader::Release()
	{
		if (myVS)
		{
			myVS->Release();
		}
		myVS = nullptr;

		if (myInputLayout)
		{
			myInputLayout->Release();
		}
		myInputLayout = nullptr;
	}

}