#include "pch.h"
#include "WindowHandler.h"
#include <Game\Postmaster.h>
#include <Engine/Input.h>
#include <Game\Globals.h>
#include <stdio.h>
#include <dwmapi.h>

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
		if (uMsg == WM_CREATE)
		{
			CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
			windowHandler = reinterpret_cast<CWindowHandler*>(createStruct->lpCreateParams);
		}

		if (uMsg == WM_DESTROY || uMsg == WM_CLOSE)
		{
			//Postmaster::GetInstance().SendMail(eMessage::eQuitGame);
			//return 0;
			PostQuitMessage(0);
		}
		else if (uMsg == WM_MOUSEWHEEL)
		{
			MSG msg = { 0 };
			msg.wParam = wParam;
			Input::HandleScrollEvent(msg);
		}
		else if (uMsg == WM_MOVE || uMsg == WM_MOVING)
		{
			windowHandler->UpdatePosition(
				{ 
					(uint)(int)(short)LOWORD(lParam), 
					(uint)(int)(short)HIWORD(lParam) 
				});
		}
		else if (uMsg == WM_SIZE || uMsg == WM_SIZING)
		{
			windowHandler->UpdateRect();
		}
		else if (uMsg == WM_EXITSIZEMOVE)
		{
			windowHandler->OnExitSizeMove();
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
			WS_OVERLAPPEDWINDOW |
			WS_POPUP |
			WS_VISIBLE |
			0u,
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

	void CWindowHandler::InitRects()
	{
		UpdatePosition({ 0u, 0u });
		UpdateRect();
		OnExitSizeMove();
	}

	void CWindowHandler::UpdateRect()
	{
		if (!myWindowHandle)
			return;

		RECT rect, frame, border;

		GetWindowRect(myWindowHandle, &rect);
		DwmGetWindowAttribute(myWindowHandle, DWMWA_EXTENDED_FRAME_BOUNDS, &frame, sizeof(RECT));

		// border
		border.left = frame.left - rect.left;
		border.top = frame.top - rect.top;
		border.right = rect.right - frame.right;
		border.bottom = rect.bottom - frame.bottom;

		// rect without borders
		rect.left += border.left + 1;
		rect.top += border.top + 1;
		rect.right -= border.right + 1;
		rect.bottom -= border.bottom * 2 + 1;

		TITLEBARINFO tbi = { 0 };
		tbi.cbSize = sizeof(TITLEBARINFO);
		if (GetTitleBarInfo(myWindowHandle, &tbi))
		{
			// move rect down
			int titleBarHeight = tbi.rcTitleBar.bottom - tbi.rcTitleBar.top;
			rect.top += titleBarHeight;
		}

		// rect dimensions
		myWindowData.width = rect.right - rect.left;
		myWindowData.height = rect.bottom - rect.top;

		Singleton<GlobalSettings>().windowRect =
		{
			(float)rect.left,
			(float)rect.top,
			(float)rect.right,
			(float)rect.bottom
		};
	}

	void CWindowHandler::UpdatePosition(uint2 aPos)
	{
		myWindowData.x = aPos.x;
		myWindowData.y = aPos.y;
		UpdateRect();
	}

	void CWindowHandler::OnExitSizeMove()
	{
		//auto rect = Singleton<GlobalSettings>().windowRect;
		//printf("x:%f, y:%f, z:%f, w:%f\n", rect.x, rect.y, rect.z, rect.w);
		//printf("x:%f, y:%f\n", (float)myWindowData.x, (float)myWindowData.y);
		Postmaster::GetInstance().SendMail(eMessage::eUpdateResolution);
	}
}
