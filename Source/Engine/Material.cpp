#include "pch.h"
#include "Material.h"
#include "TextureFactory.h"

namespace SE
{

	Material::Material()
	{
	}

	Material::Material(const Material& aMat)
	{
		myPixelShader = aMat.myPixelShader;
		myVertexShader = aMat.myVertexShader;
		myTextures = aMat.myTextures;
	}

	void Material::Reset()
	{
		myTextures.clear();
	}

	void Material::AddTexture(const std::string& aTexturePath)
	{
		myTextures.push_back(Singleton<SE::CTextureFactory>().LoadTexture(aTexturePath));
	}

	void Material::SetPS(const std::string& aPSPath)
	{
		myPixelShader.Set(aPSPath);
	}

	void Material::SetVS(const std::string& aVSPath)
	{
		myVertexShader.Set(aVSPath);
	}

	VertexShader Material::GetVS()
	{
		return myVertexShader;
	}

	PixelShader Material::GetPS()
	{
		return myPixelShader;
	}

	const std::vector<sptr(Texture)>& Material::GetTextures()
	{
		return myTextures;
	}

}