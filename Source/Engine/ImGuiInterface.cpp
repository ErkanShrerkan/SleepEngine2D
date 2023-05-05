#include "pch.h"
#include "ImGuiInterface.h"
#include <Windows.h>

// ImGui
#include <ImGui\imgui.h>
#include <ImGui\imgui_impl_win32.h>
#include <ImGui\imgui_impl_dx11.h>

//ImGuizmo
#include <ImGuizmo\ImGuizmo.h>

namespace SE
{
	// Easy HEX to normalised RGB values
	template <int hex>
	struct HexColor
	{
		const ImVec4 value = {
			static_cast<float>((hex >> 24) & 0xFF) / 255.f * static_cast<float>((hex >> 24) & 0xFF) / 255.f,
			static_cast<float>((hex >> 16) & 0xFF) / 255.f * static_cast<float>((hex >> 16) & 0xFF) / 255.f,
			static_cast<float>((hex >> 8) & 0xFF) / 255.f * static_cast<float>((hex >> 8) & 0xFF) / 255.f,
			static_cast<float>((hex >> 0) & 0xFF) / 255.f,
		};
	};

	CImGuiInterface::~CImGuiInterface()
	{
		// Cleanup
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	bool CImGuiInterface::Init(HWND hwnd, ID3D11Device* aDXDevice, ID3D11DeviceContext* aDXDeviceContext)
	{
		//IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;

		//io.IniFilename = nullptr;
		//io.IniFilename = "ImGui_Preference";
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multiple Viewports

		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(aDXDevice, aDXDeviceContext);

		ApplyDefaultStyle();

		return true;
	}

	void CImGuiInterface::ApplyDefaultStyle()
	{
#if 0
		const ImVec4 imgcNone
		(HexColor<0x00000000>().value);

		const ImVec4 imgcTextColor
		(HexColor<0xFFFFFFFF>().value);
		const ImVec4 imgcDisabledTextColor
		(HexColor<0xFFFFFF80>().value);
		const ImVec4 imgcSelectedTextColor
		(HexColor<0x8952C880>().value);

		const ImVec4 imgcTitleBG
		(HexColor<0x3D3D3DFF>().value);
		const ImVec4 imgcBorder
		(HexColor<0x303030FF>().value);
		const ImVec4 imgcMenuBarBG
		(HexColor<0x202020FF>().value);
		const ImVec4 imgcWindowBackground
		(HexColor<0x181818FF>().value);
		const ImVec4 imgcFrameBG
		(HexColor<0x121212FF>().value);

		const ImVec4 imgcActive
		(HexColor<0xBB7BB8FF>().value);
		const ImVec4 imgcHover
		(HexColor<0x8952C8FF>().value);

		const ImVec4 imgcUnknown
		(HexColor<0xFFFF0080>().value);

		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		for (size_t i = 0; i < ImGuiCol_COUNT; i++)
		{
			auto& col = colors[i];
			col.x = pow(col.x, 2.2f);
			col.y = pow(col.y, 2.2f);
			col.z = pow(col.z, 2.2f);
			//col.w = 1.f;
		}

		colors[ImGuiCol_Text] = ImColor(238, 238, 238, 255);//imgcTextColor;
		//colors[ImGuiCol_TextDisabled] = imgcDisabledTextColor;
		//colors[ImGuiCol_WindowBg] = imgcWindowBackground;
		//colors[ImGuiCol_ChildBg] = imgcNone;
		colors[ImGuiCol_PopupBg] = ImColor(25, 25, 25, 240);//imgcWindowBackground;
		//colors[ImGuiCol_Border] = imgcBorder;
		//colors[ImGuiCol_BorderShadow] = imgcNone;
		colors[ImGuiCol_FrameBg] = ImColor(1, 6, 21, 138); //imgcFrameBG;
		//colors[ImGuiCol_FrameBgHovered] = imgcFrameBG;
		//colors[ImGuiCol_FrameBgActive] = imgcFrameBG;
		colors[ImGuiCol_TitleBg] = ImColor(1, 3, 8, 255);//ImColor(HexColor<0x010308FF>().value);//imgcTitleBG;
		//colors[ImGuiCol_TitleBgActive] = imgcTitleBG;
		//colors[ImGuiCol_TitleBgCollapsed] = imgcTitleBG;
		//colors[ImGuiCol_MenuBarBg] = imgcMenuBarBG;
		//colors[ImGuiCol_ScrollbarBg] = imgcNone;
		//colors[ImGuiCol_ScrollbarGrab] = imgcMenuBarBG;
		//colors[ImGuiCol_ScrollbarGrabHovered] = imgcTitleBG;
		//colors[ImGuiCol_ScrollbarGrabActive] = imgcTitleBG;
		//colors[ImGuiCol_CheckMark] = imgcHover;
		//colors[ImGuiCol_SliderGrab] = imgcBorder;
		//colors[ImGuiCol_SliderGrabActive] = imgcHover;
		//colors[ImGuiCol_Button] = imgcFrameBG;
		//colors[ImGuiCol_ButtonHovered] = imgcHover;
		//colors[ImGuiCol_ButtonActive] = imgcActive;
		colors[ImGuiCol_Header] = ImColor(HexColor<0x4B5C854F>().value);//imgcMenuBarBG;
		colors[ImGuiCol_HeaderHovered] = ImColor(HexColor<0x2B3892CC>().value);//imgcTitleBG;
		colors[ImGuiCol_HeaderActive] = ImColor(HexColor<0x0E42E7FF>().value);//imgcTitleBG;
		colors[ImGuiCol_Separator] = ImColor(111, 111, 111, 128); //imgcBorder;
		colors[ImGuiCol_SeparatorHovered] = ImColor(HexColor<0x7CFF78C7>().value);//imgcHover;//Still unknown//ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		//colors[ImGuiCol_SeparatorActive] = imgcHover;
		//colors[ImGuiCol_ResizeGrip] = imgcTitleBG;
		//colors[ImGuiCol_ResizeGripHovered] = imgcHover;
		//colors[ImGuiCol_ResizeGripActive] = imgcActive;
		//colors[ImGuiCol_Tab] = imgcMenuBarBG;
		//colors[ImGuiCol_TabHovered] = imgcTitleBG;
		//colors[ImGuiCol_TabActive] = imgcHover;
		//colors[ImGuiCol_TabUnfocused] = imgcMenuBarBG;//ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		//colors[ImGuiCol_TabUnfocusedActive] = imgcMenuBarBG;//ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
		//colors[ImGuiCol_DockingPreview] = imgcHover;
		//colors[ImGuiCol_DockingEmptyBg] = imgcWindowBackground;
		//colors[ImGuiCol_PlotLines] = imgcHover;
		//colors[ImGuiCol_PlotLinesHovered] = imgcActive;
		//colors[ImGuiCol_PlotHistogram] = imgcHover;
		//colors[ImGuiCol_PlotHistogramHovered] = imgcActive;
		//colors[ImGuiCol_TableHeaderBg] = imgcMenuBarBG;
		//colors[ImGuiCol_TableBorderStrong] = imgcBorder;
		//colors[ImGuiCol_TableBorderLight] = imgcBorder;
		//colors[ImGuiCol_TableRowBg] = imgcWindowBackground;
		//colors[ImGuiCol_TableRowBgAlt] = imgcFrameBG;
		//colors[ImGuiCol_TextSelectedBg] = imgcSelectedTextColor;
		//colors[ImGuiCol_DragDropTarget] = imgcUnknown;//ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		//colors[ImGuiCol_NavHighlight] = imgcUnknown;//ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		//colors[ImGuiCol_NavWindowingHighlight] = imgcUnknown;//ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		//colors[ImGuiCol_NavWindowingDimBg] = imgcUnknown;//ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		//colors[ImGuiCol_ModalWindowDimBg] = imgcUnknown;//ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		//colors[ImGuiCol_ChildBg] = { .003125f, .003125f, .003125f, 1.f };

		style->FrameBorderSize = 1.f;

		style->ChildRounding = 3.f;
		style->PopupRounding = 3.f;

		style->WindowRounding = 3.f;
		style->FrameRounding = 3.f;
		style->GrabRounding = 2.f;
		style->GrabMinSize = 8.f;
		style->ScrollbarRounding = 2.f;
		style->ScrollbarSize = 14.f;

		style->IndentSpacing = 12.f;
#else
		ImVec4* colors = ImGui::GetStyle().Colors;

		colors[ImGuiCol_Header] = ImColor(HexColor<0x6E6E6E4B>().value);
		colors[ImGuiCol_DockingEmptyBg] = ImColor(HexColor<0xD2D2D254>().value);
		colors[ImGuiCol_PlotLines] = ImColor(HexColor<0x5CBC59C7>().value);
		colors[ImGuiCol_PlotLinesHovered] = ImColor(HexColor<0x5CBC59C7>().value);
		colors[ImGuiCol_PlotHistogram] = ImColor(HexColor<0x5CBC59C7>().value);
		colors[ImGuiCol_PlotHistogramHovered] = ImColor(HexColor<0x5CBC59C7>().value);
		colors[ImGuiCol_HeaderHovered] = ImColor(HexColor<0x5CBC59C7>().value); //ImColor(HexColor<0x285E1D85>().value);// Greenish
		colors[ImGuiCol_SeparatorHovered] = ImColor(HexColor<0x7CFF78C7>().value); // Light Greenish
		colors[ImGuiCol_HeaderActive] = ImColor(HexColor<0xD2D2D254>().value);
		colors[ImGuiCol_FrameBg] = ImColor(HexColor<0x00000092>().value);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);

		for (size_t i = 0; i < ImGuiCol_COUNT; i++)
		{
			auto& col = colors[i];
			col.x = pow(col.x, 1.f / 2.2f);
			col.y = pow(col.y, 1.f / 2.2f);
			col.z = pow(col.z, 1.f / 2.2f);
			//col.w = 1.f;
		}

		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button] = ImVec4(0.45f, 0.45f, 0.45f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.35f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(8.00f, 8.00f);
		style.FramePadding = ImVec2(5.00f, 2.00f);
		style.CellPadding = ImVec2(6.00f, 6.00f);
		style.ItemSpacing = ImVec2(6.00f, 6.00f);
		style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing = 12;
		style.ScrollbarSize = 15;
		style.GrabMinSize = 10;
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.FrameBorderSize = 1;
		style.TabBorderSize = 1;
		style.WindowRounding = 7;
		style.ChildRounding = 4;
		style.FrameRounding = 3;
		style.PopupRounding = 4;
		style.ScrollbarRounding = 9;
		style.GrabRounding = 3;
		style.LogSliderDeadzone = 4;
		style.TabRounding = 4;
#endif
	}

	void CImGuiInterface::BeginFrame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void CImGuiInterface::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}
