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

	class CScene
	{
	public:

		void Init();
		void AddInstance(CSprite* aSprite);
		void AddInstance(std::vector<CSprite*> someSprite);
		void PrepareFrame();
		void SetLoadingFrame(bool aLoadingFrame);
		void SetPaused(bool aValue) { myIsPaused = aValue; }

		CommonUtilities::RefillVector<CSprite*>& GetSprites();
		
	private:

		CommonUtilities::RefillVector<CSprite*> mySprites;
		
		bool myLoadingFrame = false;
		bool myIsPaused = false;
	};
}
