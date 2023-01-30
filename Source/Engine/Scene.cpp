#include "pch.h"
#include "Scene.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Camera.h"
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

	void CScene::AddInstance(CameraComponent* aCamera)
	{
		myCameras.push_back(aCamera);
	}

	void CScene::AddInstance(CSprite* aSprite)
	{
		//mySprites.push_back(aSprite);
		mySprites.push_back(aSprite);
	}

	void CScene::AddInstance(std::vector<CSprite*> someSprite)
	{
		//mySprites.push_back(aSprite);
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

	void CScene::SetMainCamera(CameraComponent* aCamera)
	{
		myMainCamera = aCamera;
	}

	CameraComponent* const& CScene::GetMainCamera()
	{
		return myMainCamera;
	}

	CommonUtilities::RefillVector<CSprite*>& CScene::GetSprites()
	{
		return mySprites;
	}
}
