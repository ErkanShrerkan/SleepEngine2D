#pragma once
#include "PixelShader.h"
#include "VertexShader.h"

namespace SE
{

	class Material
	{
	public:
		Material();
		VertexShader GetVS();
		PixelShader GetPS();

	protected:
		ID3D11VertexShader* myVertexShader = nullptr;
		ID3D11PixelShader* myPixelShader = nullptr;
	};

}
