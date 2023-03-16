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
	bool same = active == foreground/*!memcmp(&active, &foreground, sizeof(HWND))*/;
	
	isTabbed = !same;

	//if (isTabbed && same)
	//{
	//	isTabbed = false;
	//}

	Input::Update(!isTabbed);
}

void MessagePeek(MSG& aMSG)
{
	while (PeekMessage(&aMSG, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&aMSG);
		DispatchMessage(&aMSG);

		if (aMSG.message == WM_MOUSEWHEEL)
		{
			Input::ScrollEvent(aMSG);
		}
	}
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	hInstance; hPrevInstance; lpCmdLine; nCmdShow;

	InitConsole();

	SE::CEngine::SEngineParameters engineParameters{};
	engineParameters.window.x = 0;
	engineParameters.window.y = 0;
	unsigned int x = GetSystemMetrics(SM_CXSCREEN);
	unsigned int y = GetSystemMetrics(SM_CYSCREEN);
	engineParameters.window.width = static_cast<int>(x);
	engineParameters.window.height = static_cast<int>(y);
	engineParameters.window.title = L"Mire";
	engineParameters.clearColor = { 1.0f, .85f, .66f, 1.f };
	//engineParameters.clearColor = { 64.f / 255.f, 127.f / 255.f, 1.f, 1.f };
	engineParameters.clearColor = { .1f, .1f, .1f, 1.f };

	Singleton<JsonManager>().InitDocument("Data/Config.json");
	Singleton<GlobalSettings>().windowResolution = { x, y };

	// Start the Engine
	if (!SE::CEngine::GetInstance()->Start(engineParameters))
	{
		/* Error, could not start the engine */
		return 0xdead;
	}
	SE::CEngine* engine = SE::CEngine::GetInstance();


	Timer timer;
	timer.Update();
	MSG windowMessage = { 0 };

	{
		ShowCursor(false);
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

	while (isRunning)
	{
		Input::Init();
		Process* process = nullptr;
		Singleton<GlobalSettings>().gameplayResolution = (currentProcess ? uint2(960/*640*/, 540/*480*/ /*360*/) : uint2(x, y));
		isRunning = engine->Restart();

		if (currentProcess)
			process = new Game::Game();
		else
			process = new Game::Editor();

		if (!process->Init())
			return 0xbeef;

		isProcessing = true;
		while (isProcessing && isRunning)
		{
			PIXBeginEvent(PIX_COLOR_INDEX(64), __FUNCTION__);
			MessagePeek(windowMessage);
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

		delete process;
		currentProcess = !currentProcess;
		Input::DeInit();
	}

	return 0;
}
