#include "pch.h"
#include "WindowHandler.h"
#include <Game\Postmaster.h>
#include <Engine/Input.h>
#include <Game\Globals.h>

#include <stdio.h>

// ImGui Input Hijacking
#include <ImGui\imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace SE
{
	LRESULT CALLBACK CWindowHandler::WinProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
	{
		static CWindowHandler* windowHandler = nullptr;

		if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
		{
			return TRUE;
		}

		//if (uMsg == WM_DESTROY || uMsg == WM_CLOSE)
		//{
		//	//Postmaster::GetInstance().SendMail(eMessage::eQuitGame);
		//	return 0;
		//}
		if (uMsg == WM_CREATE)
		{
			CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
			windowHandler = reinterpret_cast<CWindowHandler*>(createStruct->lpCreateParams);
		}
		else if (uMsg == WM_MOUSEWHEEL)
		{
			MSG msg = { 0 };
			msg.wParam = wParam;
			Input::HandleScrollEvent(msg);
		}
		else if (uMsg == WM_SIZE/* || uMsg == WM_SIZING*/)
		{
			//RECT rect = { 0 };

			//GetWindowRect(hwnd, &rect);

			//uint x = GetSystemMetrics(SM_CXSCREEN);
			//uint y = GetSystemMetrics(SM_CYSCREEN);

			//TITLEBARINFO tbi = { 0 };
			//tbi.cbSize = sizeof(TITLEBARINFO);
			//if (GetTitleBarInfo(hwnd, &tbi))
			//{
			//	windowHandler->myWindowData.width = tbi.rcTitleBar.right - tbi.rcTitleBar.left;
			//	windowHandler->myWindowData.height = rect.bottom - rect.top - tbi.rcTitleBar.top - tbi.rcTitleBar.bottom;
			//}
			//else
			//{
			//	windowHandler->myWindowData.width = rect.right - rect.left;
			//	windowHandler->myWindowData.height = rect.bottom - rect.top;
			//}


			//Singleton<GlobalSettings>().windowRect =
			//{
			//	(float)rect.left / x,
			//	(float)rect.top / y,
			//	(float)rect.right / x,
			//	(float)rect.bottom / y
			//};

			//Postmaster::GetInstance().SendMail(eMessage::eUpdateResolution);
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	CWindowHandler::CWindowHandler()
		: myWindowHandle(nullptr)
		, myWindowData{ 0 }
	{
	}

	CWindowHandler::~CWindowHandler()
	{
		Postmaster::GetInstance().UnSubscribe(this, eMessage::e960x540);
		Postmaster::GetInstance().UnSubscribe(this, eMessage::e1280x720);
		Postmaster::GetInstance().UnSubscribe(this, eMessage::e1600x900);
		Postmaster::GetInstance().UnSubscribe(this, eMessage::e1920x1080);
		Postmaster::GetInstance().UnSubscribe(this, eMessage::e2560x1440);
	}

	void CWindowHandler::RecieveMessage(eMessage aMsg)
	{
		if (static_cast<unsigned>(GetSystemMetrics(SM_CYSCREEN)) >= static_cast<unsigned>(aMsg))
		{
			myWindowData.width = static_cast<unsigned>((16.f / 9) * static_cast<unsigned>(aMsg));
			myWindowData.height = static_cast<unsigned>(aMsg);
			Postmaster::GetInstance().SendMail(eMessage::eUpdateResolution);
		}
	}

	bool CWindowHandler::Init(const SWindowData& someData)
	{
		myWindowData = someData;

		WNDCLASS windowClass = {};
		windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		windowClass.lpfnWndProc = CWindowHandler::WinProc;
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.lpszClassName = L"SleepEngine";
		RegisterClass(&windowClass);

		myWindowHandle = CreateWindow(L"SleepEngine", myWindowData.title,
			//WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE,
			/*WS_OVERLAPPEDWINDOW |*/ WS_POPUP | WS_VISIBLE,
			myWindowData.x, myWindowData.y, myWindowData.width, myWindowData.height,
			nullptr, nullptr, nullptr, this);

		return true;
	}

	void CWindowHandler::InitSubscribtions()
	{
		Postmaster::GetInstance().Subscribe(this, eMessage::e960x540);
		Postmaster::GetInstance().Subscribe(this, eMessage::e1280x720);
		Postmaster::GetInstance().Subscribe(this, eMessage::e1600x900);
		Postmaster::GetInstance().Subscribe(this, eMessage::e1920x1080);
		Postmaster::GetInstance().Subscribe(this, eMessage::e2560x1440);
	}

	HWND& CWindowHandler::GetWindowHandle()
	{
		return myWindowHandle;
	}
	const int CWindowHandler::GetWidth() const
	{
		return myWindowData.width;
	}
	const int CWindowHandler::GetHeight() const
	{
		return myWindowData.height;
	}
}
