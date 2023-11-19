#pragma once
//#include <Windows.h>
//#include <functional>

#include <CommonUtilities\Math\Vector4.hpp>

/*

	Use this as MainSingleton with sub "singletons"

*/

#include "WindowHandler.h" // Struct

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ImGuiContext;
class GameManager;

namespace SE
{
	//using Callback			= std::function<void()>;
	//using Callback_WndProc	= std::function<LRESULT(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)>;

	class CContentLoader;
	class CGraphicsEngine;
	class CAudioEngine;
	class CScene;
	class CEditor;
	class CEngine
	{
	public:
		struct SEngineParameters
		{
			CWindowHandler::SWindowData window{};
			CommonUtilities::Vector4<float> clearColor;
		};

		static CEngine* GetInstance() 
		{ 
			static CEngine instance;
			return &instance;
		}

	private:
		CEngine() {}
		float myDeltaTime;

	public:
		bool Start(const SEngineParameters& someParameters);
		bool Restart();

		//CEngine(const SEngineParameters& someParameters);
		~CEngine();

		bool Update(float aDeltaTime);
		void Render();
		void BeginFrame();
		void EndFrame();

		void SetActiveScene(CScene* const& aScene);
		CScene* const& GetActiveScene();
		ID3D11Device* const& GetDXDevice() const;
		ID3D11DeviceContext* const& GetDXDeviceContext() const;
		CGraphicsEngine* const& GetGraphicsEngine();
		CContentLoader* const& GetContentLoader();
		CWindowHandler& GetWindowHandler();
		float GetDeltaTime() { return myDeltaTime; };
		void SetGameManagerRef(GameManager* aGameManagerRef) { myGMptr = aGameManagerRef; }
		GameManager& GetGameManager()
		{
			return *myGMptr;
		}
	private:
		bool InternalStart();

		SEngineParameters myEngineParameters{};

		CContentLoader* myContentLoader = nullptr;
		CGraphicsEngine* myGraphicsEngine = nullptr;
		CAudioEngine* myAudioEngine = nullptr;
		GameManager* myGMptr = nullptr;
		CScene* myScene = nullptr;
	};
}
