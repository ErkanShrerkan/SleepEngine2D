#include "pch.h"
#include "DebugProfiler.h"

#ifdef _DEBUG
#include <ddkernel.h>			// K32GetProcessMemoryInfo
#include <psapi.h>				// GetProcessMemoryInfo
#include <processthreadsapi.h>	// GetCurrentProcess

#include "Engine.h"
#include <Game\Time.h>

int g_new;
int g_del;
int g_dif;
void* operator new(size_t size)
{
	++g_new;
	++g_dif;

	// Use this if statement when
	// you're wondering where all
	// them allocations come from
	//if (size >= 10'000'000)
	//{
	//	int d = 5;
	//	*reinterpret_cast<float*>(&d) = 5.0f;
	//}

	return malloc(size);
}
void operator delete(void* address)
{
	++g_del;
	--g_dif;
	free(address);
}
#endif // _DEBUG

namespace SE
{
	namespace Debug
	{
		CDebugProfiler::CDebugProfiler()
			: myClock()
			, texture("Textures/art.dds")
			, myDrawCallCountSum(0)
			, myDrawCallCount(0)
		{
			//Debug::Track(new int, __FILE__);

			//ImGui::Image()
		}

		void CDebugProfiler::BeginCapture()
		{
			#ifdef _DEBUG
			myFirstTime = myClock.now();
			myDrawCallCountSum += myDrawCallCount;
			myDrawCallCount = 0;
			myDrawnTriangles = 0;
			#endif // _DEBUG
		}

		void CDebugProfiler::EndCapture()
		{
			#ifdef _DEBUG
			float deltaTime = std::chrono::duration<float>(myClock.now() - myFirstTime).count();
			deltaTime;
			myTotalFrameTime += Singleton<Time>().deltaTime;
			myFrameCounter += 1.f;

			PROCESS_MEMORY_COUNTERS memCounter;
			GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));
			myMemoryUsage = static_cast<int>(memCounter.WorkingSetSize) / 1024;

			myTimer += deltaTime;
			if (myTimer >= myRefreshRate)
			{
				const float oneDividedByFrameCounter = 1.0f / myFrameCounter;

				myAverageTime = myTotalFrameTime * oneDividedByFrameCounter;
				myFrameCounter = 0.f;
				myTotalFrameTime = 0.f;
				myTimer = 0.f;

				int oldAverage = myAverageFPS;

				myAverageFPS = static_cast<int>(1.f / myAverageTime);
				myAverageTime *= 1000.f;

				myFrameDrops.Push(static_cast<float>(myAverageFPS - oldAverage));

				myMallocDiff = g_new - myOldMallocs;
				myOldMallocs = g_new;

				myAllocations.Push(static_cast<float>(myMallocDiff));
				//myAllocations.Push(static_cast<float>(g_dif));
				//myAllocations.Push(static_cast<float>(myMemoryUsage) / 1000.f);
				myFrameRates.Push(static_cast<float>(myAverageFPS));
				myAverageTimes.Push(myAverageTime);

				myAverageDrawCalls.Push(static_cast<float>(myDrawCallCountSum) * oneDividedByFrameCounter);
				myDrawCallCountSum = 0;
			}
			#endif // _DEBUG
		}

		void CDebugProfiler::Render()
		{
			#ifdef _DEBUG
			ImGui::Text("");
			ImGui::Text("Profiler");
			ImGui::Separator();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			//ImGui::SetNextWindowSize(ImVec2(350, 675), ImGuiCond_FirstUseEver);
			//if (ImGui::Begin("Profiler##prf", 0, ImGuiWindowFlags_NoScrollbar))

			{
				ImGui::Text("Total Mallocs: %i", g_new);
				ImGui::Text("Total Frees: %i", g_del);
				ImGui::Text("Last Diff: %i", myMallocDiff);
				ImGui::Text("Memory Usage: %i", myMemoryUsage);
				ImGui::Text("Avg MS: %f", myAverageTime);
				ImGui::Text("Avg FPS: %i", myAverageFPS);
				ImGui::Text("Draw Calls: %i", myDrawCallCount);
				ImGui::Text("Drawn Triangles: %i", myDrawnTriangles);

				//ImGui::Image((ImTextureID)texture.GetShaderResourceView(), ImVec2(texture.GetWidth(), texture.GetWidth()));

				ImGui::Text("Data Refresh Rate: %.3f seconds", myRefreshRate);

				if (ImGui::BeginTable("profiler-plots", 1))
				{
					// FLT_MAX in plotlines/plothistogram removes a cap and automates the levels for us

					ImGui::SetNextItemWidth(-1);
					ImGui::TableNextColumn();
					ImGui::PlotHistogram("##", myAllocations.Get(), myAllocations.Size(), 0, "Memory Usage", 0.f, FLT_MAX, ImVec2(0, 80.f));

					ImGui::TableNextRow();
					ImGui::SetNextItemWidth(-1);
					ImGui::TableNextColumn();
					ImGui::PlotHistogram("##", myAverageTimes.Get(), myAverageTimes.Size(), 0, "Average MS Per Frame", 0.f, FLT_MAX, ImVec2(0, 60.f));

					ImGui::TableNextRow();
					ImGui::SetNextItemWidth(-1);
					ImGui::TableNextColumn();
					ImGui::PlotHistogram("##", myFrameRates.Get(), myFrameRates.Size(), 0, "Average FPS", 0.f, FLT_MAX, ImVec2(0, 100.f));

					/*ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextUnformatted("FPS Stability");*/
					ImGui::TableNextRow();
					ImGui::SetNextItemWidth(-1);
					ImGui::TableNextColumn();
					ImGui::PlotLines("##", myFrameDrops.Get(), myFrameDrops.Size(), 0, "FPS Stability", FLT_MAX, FLT_MAX, ImVec2(0, 100.f));
					
					/*ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::TextUnformatted("Average Draw Calls");*/
					ImGui::TableNextRow();
					ImGui::SetNextItemWidth(-1);
					ImGui::TableNextColumn();
					ImGui::PlotLines("##", myAverageDrawCalls.Get(), myAverageDrawCalls.Size(), 0, "Average Draw Calls", FLT_MAX, FLT_MAX, ImVec2(0, 100.f));

					ImGui::EndTable();
				}
			}
			ImGui::PopStyleVar();
			//ImGui::End();
			#endif // _DEBUG
		}
		void CDebugProfiler::IncrementDrawCallCount()
		{
			#ifdef _DEBUG
			++myDrawCallCount;
			#endif // _DEBUG
		}
		void CDebugProfiler::IncrementDrawnTriangles(int anAmount)
		{
			#ifdef _DEBUG
			myDrawnTriangles += anAmount;
			#else
			anAmount;
			#endif // _DEBUG
		}
	}
}
