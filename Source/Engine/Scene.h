#pragma once
#include "ShaderDefines.h"
#include <array>
#include <Engine\Transform.h>
#include <memory>
#include <map>
#include <CommonUtilities\Container\RefillVector.h>
// TODO: Replace vector with set
class Text;
namespace SE
{
	class CSprite;
	class CameraComponent;

	class CScene
	{
	public:

		void Init();
		void AddInstance(CameraComponent* aCamera);
		void AddInstance(CSprite* aSprite);
		void AddInstance(std::vector<CSprite*> someSprite);
		void PrepareFrame();
		void SetLoadingFrame(bool aLoadingFrame);
		void SetPaused(bool aValue) { myIsPaused = aValue; }

		void SetMainCamera(CameraComponent* aCamera);
		CameraComponent* const& GetMainCamera();

		CommonUtilities::RefillVector<CSprite*>& GetSprites();
		
	private:

		CameraComponent* myMainCamera = nullptr;
		std::vector<CameraComponent*> myCameras;
		CommonUtilities::RefillVector<CSprite*> mySprites;
		
		bool myLoadingFrame = false;
		bool myIsPaused = false;
	};
}
