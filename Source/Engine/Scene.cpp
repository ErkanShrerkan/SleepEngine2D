#include "pch.h"
#include "Scene.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "EnvironmentLight.h"
#include <Game\macro.h>
#include "LineDrawer.h"
#include <d3d11.h>
#include <Engine\Engine.h>

namespace SE
{
	void CScene::Init()
	{

	}

	void CScene::AddInstance(Sprite* aSprite)
	{
		mySprites.push_back(aSprite);
	}

	void CScene::AddInstance(std::vector<Sprite*> someSprite)
	{
		for (auto& sprite : someSprite)
		{
			AddInstance(sprite);
		}
	}

	void CScene::AddInstance(CSprite* aSprite)
	{
		mySSSprites.push_back(aSprite);
	}

	void CScene::AddInstance(std::vector<CSprite*> someSprite)
	{
		for (auto& sprite : someSprite)
		{
			AddInstance(sprite);
		}
	}

	void CScene::PrepareFrame()
	{
		mySprites.flush();
	}

	void CScene::SetLoadingFrame(bool aLoadingFrame)
	{
		myLoadingFrame = aLoadingFrame;
	}

	CommonUtilities::RefillVector<Sprite*>& CScene::GetSprites()
	{
		return mySprites;
	}
	CommonUtilities::RefillVector<CSprite*>& CScene::GetSSSprites()
	{
		return mySSSprites;
	}
}
