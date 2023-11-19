#include "pch.h"
#include <windows.h>
#include <Engine/Engine.h>
#include <Engine/Timer.h>
#include <cmath>
#include <string>
#include <Game/Game.h>
#include "Engine/Input.h"
#define USE_PIX 1
#include "pix3.h"
#include <Engine\DX11.h>
#include <Engine\GraphicsEngine.h>
#include <Engine\WindowHandler.h>
#include <Engine\SplashScreen.h>
#include <Engine\TextureHelper.h>
#include <Engine\WindowHandler.h>

using namespace CommonUtilities;

#include <fcntl.h>
#include <filesystem>
#include <Engine\Async.h>
#include <Engine\ContentLoader.h>
#include <Engine\EnvironmentLight.h>
#include <Game\JsonManager.h>
#include <Game\Editor.h>
#include <Game\Globals.h>
#include <Game\Process.h>
#include <Game\Time.h>

std::mutex mutex;
std::condition_variable condition;

void InitConsoleToFile()
{
	FILE* fDummy;
	freopen_s(&fDummy, "console_log.txt", "w", stdout);
}

void InitConsole()
{
	AllocConsole();
	FILE* fDummy;
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	SetConsoleTitle(L"Print Logger for Sleep Engine");
	printf("Use pout instead of printf as this window is used in case of an unexpected crash rather than as an actual debugger.\n");
	printf("=======================\n");
}

void HandleTabbing(bool& isTabbed)
{
	HWND active = GetActiveWindow();
	HWND foreground = GetForegroundWindow();
	bool same = active == foreground;
	isTabbed = !same;
	Input::Update(!isTabbed);
}

bool MessagePeek(MSG& aMSG)
{
	int i = 0;
	while (PeekMessage(&aMSG, 0, 0, 0, PM_REMOVE))
	{
		if (i > 1000)
		{
			return false;
		}

		TranslateMessage(&aMSG);
		DispatchMessage(&aMSG);

		if (aMSG.message == WM_QUIT)
		{
			return false;
		}

		i++;
	}
	return true;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	hInstance; hPrevInstance; lpCmdLine; nCmdShow;

	InitConsole();

	SE::CEngine* engine = SE::CEngine::GetInstance();
	uint x = uint((float)GetSystemMetrics(SM_CXSCREEN) * .8f);
	uint y = uint((float)GetSystemMetrics(SM_CYSCREEN) * .8f);

	auto& gs = Singleton<GlobalSettings>();

	{
		SE::CEngine::SEngineParameters engineParameters{};
		engineParameters.window.x = 0;
		engineParameters.window.y = 0;
		engineParameters.window.width = static_cast<int>(x);
		engineParameters.window.height = static_cast<int>(y);
		engineParameters.window.title = L"Sleep Engine 2D";
		engineParameters.clearColor = { 1.0f, .85f, .66f, 1.f };
		//engineParameters.clearColor = { 64.f / 255.f, 127.f / 255.f, 1.f, 1.f };
		engineParameters.clearColor = { .1f, .1f, .1f, 1.f };

		gs.gameplayResolution = { 640, /*480*/ 360 };
		gs.windowResolution = { x, y };
		gs.gameWindowRect = { 0.f, 0.f, (float)x, (float)y };
		gs.windowRect = { 0.f, 0.f, (float)x, (float)y };
		gs.screenResolution =
		{
			(uint)GetSystemMetrics(SM_CXSCREEN),
			(uint)GetSystemMetrics(SM_CYSCREEN)
		};

		// Start the Engine
		if (!engine->Start(engineParameters))
		{
			/* Error, could not start the engine */
			return 0xdead;
		}
		engine->GetGraphicsEngine()->GetWindowHandler().InitRects();
	}

	Timer timer;
	timer.Update();
	MSG windowMessage = { 0 };

	//ShowCursor(false);

	{
#if 0
		SplashScreen* ss = new SplashScreen();
		bool isSplashing = true;
		while (isSplashing)
		{
			while (PeekMessage(&windowMessage, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&windowMessage);
				DispatchMessage(&windowMessage);

				if (windowMessage.message == WM_QUIT)
				{
					isRunning = false;
				}
			}

			timer.Update();
			float dt = timer.GetDeltaTime();

			// Main Loop
			engine->BeginFrame();
			ss->Update(dt);
			ss->Render();
			engine->Render();
			engine->EndFrame();

			isSplashing = ss->GetState() != SplashScreen::eState::Over;
		}
		delete ss;
#endif // RELEASE
	}

	//Input::Init();
	//Input::LockCursor();
	timer.Update();

	bool isRunning = true;
	bool isProcessing = true;
	bool isTabbed = false;
	bool currentProcess = false;

	// TODO: make splash screen a process
	while (isRunning)
	{
		Input::Init();
		sptr(Process) process;

		Singleton<GlobalSettings>().gameplayResolution =
			(currentProcess ? uint2(960 / 2/*640*/, 540 / 2/*480*/ /*360*/) : gs.windowResolution);

		isRunning = engine->Restart();

		if (currentProcess)
			process = std::make_shared<Game::Game>();
		else
			process = std::make_shared<Game::Editor>();

		if (!process->Init())
			return 0xbeef;

		isProcessing = true;
		while (isProcessing && isRunning)
		{
			PIXBeginEvent(PIX_COLOR_INDEX(64), __FUNCTION__);
			isRunning = MessagePeek(windowMessage);
			HandleTabbing(isTabbed);

			engine->BeginFrame();
			timer.Update();
			float dt = timer.GetDeltaTime();
			Singleton<Time>().deltaTime = dt;
			engine->Update(dt);
			isProcessing = process->Update();
			engine->Render();
			engine->EndFrame();

			Input::ResetScrollInput();
			PIXEndEvent();

			if (Input::GetInputDown(eInputEvent::Escape))
			{
				isRunning = false;
			}
		}

		currentProcess = !currentProcess;
		Input::DeInit();
	}

	return 0;
}
