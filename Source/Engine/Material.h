#pragma once
#include "PixelShader.h"
#include "VertexShader.h"

namespace SE
{
	class Texture;
	class Material
	{
	public:
		Material();
		void Reset();
		void AddTexture(const std::string& aTexturePath);
		void SetPS(const std::string& aPSPath);
		void SetVS(const std::string& aVSPath);
		VertexShader GetVS();
		PixelShader GetPS();
		const std::vector<sptr(Texture)>& GetTextures();

	protected:
		VertexShader myVertexShader;
		PixelShader myPixelShader;
		std::vector<sptr(Texture)> myTextures;
	};

}
