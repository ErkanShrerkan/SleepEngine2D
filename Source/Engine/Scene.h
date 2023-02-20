#pragma once
#include "ShaderDefines.h"
#include <array>
#include <Engine\Transform.h>
#include <memory>
#include <map>
#include <CommonUtilities\Container\RefillVector.h>
// TODO: Replace vector with set
class Text;
class Sprite;
namespace SE
{
	class CSprite;
	class CScene
	{
	public:

		void Init();
		void AddInstance(Sprite* aSprite);
		void AddInstance(std::vector<Sprite*> someSprite);
		void PrepareFrame();
		void SetLoadingFrame(bool aLoadingFrame);
		void SetPaused(bool aValue) { myIsPaused = aValue; }

		CommonUtilities::RefillVector<Sprite*>& GetSprites();
		CommonUtilities::RefillVector<Sprite*>& GetSSSprites();
		
	private:

		CommonUtilities::RefillVector<Sprite*> mySprites;
		CommonUtilities::RefillVector<Sprite*> mySSSprites;
		
		bool myLoadingFrame = false;
		bool myIsPaused = false;
	};
}
