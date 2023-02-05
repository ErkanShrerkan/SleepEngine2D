#include "pch.h"
#include "Engine.h"
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#define USE_PIX 1
#include "pix3.h"
#include "GraphicsEngine.h"
#include "ContentLoader.h"
#include "DebugProfiler.h"
#include "Editor.h"
// Shortcuts
#include "DX11.h"
#include "AudioEngine.h"
#include "Scene.h"
#include <Game\GameManager.h>
#include <Engine\TextureHelper.h>

namespace SE
{
	bool CEngine::Start(const SEngineParameters& someParameters)
	{
		myEngineParameters = someParameters;
		myGraphicsEngine = nullptr;
		myContentLoader = new CContentLoader();
		DX11::Content = myContentLoader;
		myScene = new CScene();
		myAudioEngine = CAudioEngine::GetInstance();
		return InternalStart();
	}

	bool CEngine::Restart()
	{
		return myGraphicsEngine->Restart();
	}

	CEngine::~CEngine()
	{
		// Release resources
		
		Helper::TextureHelper::TextureBank::GetInstance().Release();

		delete myContentLoader;
		myContentLoader = nullptr;

		delete myGraphicsEngine;
		myGraphicsEngine = nullptr;

		delete myScene;
		myScene = nullptr;
	}

	bool CEngine::Update(float aDeltaTime)
	{
		PIXBeginEvent(PIX_COLOR_INDEX(0), __FUNCTION__);

		myDeltaTime = aDeltaTime;

		PIXEndEvent();
		return true;
	}

	void CEngine::Render()
	{
		PIXBeginEvent(PIX_COLOR_INDEX(0), __FUNCTION__);

		//*///
		myGraphicsEngine->RenderFrame();
		//*///

		/*///
		//PIXBeginEvent(PIX_COLOR_INDEX(1), "Debug::CDebugConsole::Render");
		pout.Render();
		//PIXEndEvent();
		//*///

		//*///
		PIXBeginEvent(PIX_COLOR_INDEX(1), "Debug::CDebugProfiler::Render");
		Singleton<Debug::CDebugProfiler>().Render();
		PIXEndEvent();
		//*///

		/*///
		//PIXBeginEvent(PIX_COLOR_INDEX(1), "CEditor::Render");
		myEditor->Render();
		//PIXEndEvent();
		//*///

		PIXEndEvent();
	}

	void CEngine::BeginFrame()
	{
		PIXBeginEvent(PIX_COLOR_INDEX(0), __FUNCTION__);
		GetActiveScene()->PrepareFrame();
		Singleton<Debug::CDebugProfiler>().BeginCapture();
		myGraphicsEngine->BeginFrame();
		PIXEndEvent();
	}

	void CEngine::EndFrame()
	{
		PIXBeginEvent(PIX_COLOR_INDEX(0), __FUNCTION__);
		myGraphicsEngine->EndFrame();
		Singleton<Debug::CDebugProfiler>().EndCapture();
		PIXEndEvent();
	}

	void CEngine::SetActiveScene(CScene* const& aScene)
	{
		myScene = aScene;
	}

	CScene* const& CEngine::GetActiveScene()
	{
		return myScene;
	}

	ID3D11Device* const& CEngine::GetDXDevice() const
	{
		return myGraphicsEngine->GetFramework().GetDevice();
	}

	ID3D11DeviceContext* const& CEngine::GetDXDeviceContext() const
	{
		return myGraphicsEngine->GetFramework().GetContext();
	}

	CGraphicsEngine* const& CEngine::GetGraphicsEngine()
	{
		return myGraphicsEngine;
	}

	CContentLoader* const& CEngine::GetContentLoader()
	{
		return myContentLoader;
	}

	bool CEngine::InternalStart()
	{
		myGraphicsEngine = new CGraphicsEngine();
		DX11::Graphics = myGraphicsEngine;
		bool shouldRun = myGraphicsEngine->Init(myEngineParameters.window);
		if (!shouldRun)
		{
			/* Error Message */
			return false;
		}
		myGraphicsEngine->SetClearColor(myEngineParameters.clearColor);

		if (!shouldRun)
		{
			/* Error Message */
			return false;
		}

		return true;
	}
}
