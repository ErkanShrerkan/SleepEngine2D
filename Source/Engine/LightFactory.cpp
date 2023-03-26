#include "pch.h"
#include "LightFactory.h"
#include "EnvironmentLight.h"
#include "PointLight.h"
#include "SpotLight.h"

// To get Cubemap
#include "Texture.h"
#include "TextureHelper.h"
#include "Camera.h"
#include "Engine.h"
#include "ContentLoader.h"

/*
 * Good settings for point lights:
 * ---------------{  X    Y    Z    W  } {   R     G     B    A  }
 * Red   : Fire   { 114, 144, -92, 160 } { .905, .105, .005,  50 }
 * Green : Goblet { -56,  21, -69,  53 } { .005, .265, .035,   2 }
 * Blue  : Magic  {   0,   0,  57,  84 } { .050, .025, .900,   4 }
 */

namespace SE
{
	CLightFactory::~CLightFactory()
	{
		for (auto& light : myEnvironmentLights)
		{
			delete light;
		}
	}

	CEnvironmentLight* CLightFactory::CreateEnvironmentLight(const std::string& aPath)
	{
		aPath;
		CEnvironmentLight* light = new CEnvironmentLight();

		////CTexture texture = CTexture(aPath);
		//light->myCubemap = texture.GetShaderResourceView();
		//light->myMipLevels = texture.GetMipLevels();

		std::shared_ptr<CCamera> cam = std::make_shared<CCamera>();
		float w = 100'00;
		cam->Init({ w, w });
		light->AddProjection(cam->GetProjection());
		cam->Init({ w * 11, w * 11 });
		light->AddProjection(cam->GetProjection());
		cam->Init({ w, w });
		light->myShadowCam = cam;

		myEnvironmentLights.push_back(light);

		return light;
	}

	CPointLight* CLightFactory::CreatePointLight()
	{
		CPointLight* light = new CPointLight();

		return light;
	}

	CSpotLight* CLightFactory::CreateSpotLight()
	{
		CSpotLight* light = new CSpotLight();

		return light;
	}
}
