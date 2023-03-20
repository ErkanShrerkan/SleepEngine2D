#pragma once
#include <memory>
// Factories
#include "LightFactory.h"
#include "TextureFactory.h"
#include "SpriteFactory.h"
namespace SE
{
	// TODO: Remove overloaded load functions and
	// replace them with "GetXXXFactory()" instead

	class CContentLoader
	{
	public:
		// Use an overloaded version of this to load desired resource
		template <class Resource, class ...Args>
		Resource* Load(Args... someArguments);

		CFullscreenTexture Load(const Vector2ui& aSize, DXGI_FORMAT aFormat);
		CFullscreenTexture Load(ID3D11Texture2D* aTexture);

		SE::CLightFactory& GetLightFactory();
		SE::CTextureFactory& GetTextureFactory();
		SE::CSpriteFactory& GetSpriteFactory();

	private:
		SE::CLightFactory myLightFactory;
		SE::CTextureFactory myTextureFactory;
		SE::CSpriteFactory mySpriteFactory;
	};

	template<class Resource, class ...Args>
	inline Resource* CContentLoader::Load(Args ...someArguments)
	{
		static_assert(false, "SE::CContentLoader<Resource>::Load() was called with an unsupported type or unsupported arguments");
		return nullptr;
	}

#define _MSE_LOAD_CONTENT(Type, ...) template <> inline Type* CContentLoader::Load<Type>(__VA_ARGS__)


	// Create environtment light
	_MSE_LOAD_CONTENT(SE::CEnvironmentLight, const char* aPath)
	{
		return myLightFactory.CreateEnvironmentLight(aPath);
	}
	// Create point light
	_MSE_LOAD_CONTENT(SE::CPointLight)
	{
		return myLightFactory.CreatePointLight();
	}
	// Create spot light
	_MSE_LOAD_CONTENT(SE::CSpotLight)
	{
		return myLightFactory.CreateSpotLight();
	}

#undef _MSE_CONTENT_LOAD
}
