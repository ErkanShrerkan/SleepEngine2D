#include "pch.h"
#include "Scene.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "EnvironmentLight.h"
#include <Game\macro.h>
#include "LineDrawer.h"
#include <d3d11.h>
#include <Engine\Engine.h>
#include <Game\Sprite.h>

namespace SE
{
	void CScene::Init()
	{

	}

	void CScene::AddInstance(Sprite* aSprite)
	{
		if (aSprite->GetSpace())
		{
			mySSSprites.push_back(aSprite);
			return;
		}

		mySprites.push_back(aSprite);
	}

	void CScene::AddInstance(std::vector<Sprite*> someSprite)
	{
		for (auto& sprite : someSprite)
		{
			AddInstance(sprite);
		}
	}

	void CScene::PrepareFrame()
	{
		mySprites.flush();
		mySSSprites.flush();
	}

	void CScene::SetLoadingFrame(bool aLoadingFrame)
	{
		myLoadingFrame = aLoadingFrame;
	}

	CommonUtilities::RefillVector<Sprite*>& CScene::GetSprites()
	{
		return mySprites;
	}

	CommonUtilities::RefillVector<Sprite*>& CScene::GetSSSprites()
	{
		return mySSSprites;
	}
}
