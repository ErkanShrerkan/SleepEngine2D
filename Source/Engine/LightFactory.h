#pragma once
#include <string_view>
namespace SE
{
	class CEnvironmentLight; // CDirectionalLight???
	class CPointLight;
	class CSpotLight;
	class CLightFactory
	{
	public:
		~CLightFactory();
		CEnvironmentLight* CreateEnvironmentLight(const std::string& aPath);
		CPointLight* CreatePointLight();
		CSpotLight* CreateSpotLight();

	private:
		std::vector<CEnvironmentLight*> myEnvironmentLights;
	};
}
