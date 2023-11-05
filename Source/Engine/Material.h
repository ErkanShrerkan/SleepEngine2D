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
		VertexShader myVertexShader;
		PixelShader myPixelShader;
	};

}
