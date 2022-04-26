#pragma once
#include "imgui.h"
#include <math.h>
#include <iostream>
#include <imgui-SFML.h>

class EditorColorScheme
{
	// 0xRRGGBBAA
	inline static int BackGroundColor = 0x25213100;
	inline static int TextColor = 0xF4F1DE00;
	inline static int MainColor = 0xDA115E00;
	inline static int MainAccentColor = 0x79235900;
	inline static int HighlightColor = 0xC7EF0000;

	inline static int Black = 0x00000000;
	inline static int White = 0xFFFFFF00;

	inline static int AlphaTransparent = 0x00;
	inline static int Alpha20 = 0x33;
	inline static int Alpha40 = 0x66;
	inline static int Alpha50 = 0x80;
	inline static int Alpha60 = 0x99;
	inline static int Alpha80 = 0xCC;
	inline static int Alpha90 = 0xE6;
	inline static int AlphaFull = 0xFF;

	inline static int BorderWidth = 0;

	static float GetR(int colorCode) { return (float)((colorCode & 0xFF000000) >> 24) / (float)(0xFF); }
	static float GetG(int colorCode) { return (float)((colorCode & 0x00FF0000) >> 16) / (float)(0xFF); }
	static float GetB(int colorCode) { return (float)((colorCode & 0x0000FF00) >> 8) / (float)(0xFF); }
	static float GetA(int alphaCode) { return ((float)alphaCode / (float)0xFF); }

	static ImVec4 GetColor(int c, int a = Alpha80) { return ImVec4(GetR(c), GetG(c), GetB(c), GetA(a)); }
	static ImVec4 Darken(ImVec4 c, float p) { return ImVec4(fmax(0.f, c.x - 1.0f * p), fmax(0.f, c.y - 1.0f * p), fmax(0.f, c.z - 1.0f * p), c.w); }
	static ImVec4 Lighten(ImVec4 c, float p) { return ImVec4(fmax(0.f, c.x + 1.0f * p), fmax(0.f, c.y + 1.0f * p), fmax(0.f, c.z + 1.0f * p), c.w); }

	static ImVec4 Disabled(ImVec4 c) { return Darken(c, 0.6f); }
	static ImVec4 Hovered(ImVec4 c) { return Lighten(c, 0.2f); }
	static ImVec4 Active(ImVec4 c) { return Lighten(ImVec4(c.x, c.y, c.z, 1.0f), 0.1f); }
	static ImVec4 Collapsed(ImVec4 c) { return Darken(c, 0.2f); }

public:

	struct Theme
	{
		int BackGroundColor;
		int TextColor;
		int MainColor;
		int MainAccentColor;
		int HighlightColor;
		int BorderWidth;
	};


	static void SetColors(int backGroundColor, int textColor, int mainColor, int mainAccentColor, int highlightColor, bool border = false)
	{
		BackGroundColor = backGroundColor;
		TextColor = textColor;
		MainColor = mainColor;
		MainAccentColor = mainAccentColor;
		HighlightColor = highlightColor;
		BorderWidth = border;
	}

	static Theme GetColors()
	{
		Theme cur = { BackGroundColor, TextColor, MainColor, MainAccentColor, HighlightColor, BorderWidth };
		return cur;
	}

	static void ApplyFonts()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF("resources\\Fonts\\Roboto-Bold.ttf", 16.f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("resources\\Fonts\\Roboto-Regular.ttf", 16.f);
		ImGui::SFML::UpdateFontTexture();
	}

	static void ApplyTheme()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;

		colors[ImGuiCol_Text] = GetColor(TextColor);
		colors[ImGuiCol_TextDisabled] = Disabled(colors[ImGuiCol_Text]);
		colors[ImGuiCol_WindowBg] = GetColor(BackGroundColor);
		colors[ImGuiCol_ChildBg] = GetColor(Black, Alpha20);
		colors[ImGuiCol_PopupBg] = GetColor(MainAccentColor, Alpha90);
		colors[ImGuiCol_Border] = Lighten(GetColor(BackGroundColor), 0.4f);
		colors[ImGuiCol_BorderShadow] = GetColor(Black);
		colors[ImGuiCol_FrameBg] = GetColor(MainAccentColor, Alpha40);
		colors[ImGuiCol_FrameBgHovered] = Hovered(colors[ImGuiCol_FrameBg]);
		colors[ImGuiCol_FrameBgActive] = Active(colors[ImGuiCol_FrameBg]);
		colors[ImGuiCol_TitleBg] = GetColor(BackGroundColor, Alpha80);
		colors[ImGuiCol_TitleBgActive] = Active(colors[ImGuiCol_TitleBg]);
		colors[ImGuiCol_TitleBgCollapsed] = Collapsed(colors[ImGuiCol_TitleBg]);
		colors[ImGuiCol_MenuBarBg] = Darken(GetColor(BackGroundColor), 0.2f);
		colors[ImGuiCol_ScrollbarBg] = Lighten(GetColor(BackGroundColor, Alpha50), 0.4f);
		colors[ImGuiCol_ScrollbarGrab] = Lighten(GetColor(BackGroundColor), 0.3f);
		colors[ImGuiCol_ScrollbarGrabHovered] = Hovered(colors[ImGuiCol_ScrollbarGrab]);
		colors[ImGuiCol_ScrollbarGrabActive] = Active(colors[ImGuiCol_ScrollbarGrab]);
		colors[ImGuiCol_CheckMark] = GetColor(HighlightColor);
		colors[ImGuiCol_SliderGrab] = GetColor(HighlightColor);
		colors[ImGuiCol_SliderGrabActive] = Active(colors[ImGuiCol_SliderGrab]);
		colors[ImGuiCol_Button] = GetColor(MainColor, Alpha80);
		colors[ImGuiCol_ButtonHovered] = Hovered(colors[ImGuiCol_Button]);
		colors[ImGuiCol_ButtonActive] = Active(colors[ImGuiCol_Button]);
		colors[ImGuiCol_Header] = GetColor(MainAccentColor, Alpha80);
		colors[ImGuiCol_HeaderHovered] = Hovered(colors[ImGuiCol_Header]);
		colors[ImGuiCol_HeaderActive] = Active(colors[ImGuiCol_Header]);
		colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
		colors[ImGuiCol_SeparatorHovered] = Hovered(colors[ImGuiCol_Separator]);
		colors[ImGuiCol_SeparatorActive] = Active(colors[ImGuiCol_Separator]);
		colors[ImGuiCol_ResizeGrip] = GetColor(MainColor, Alpha20);
		colors[ImGuiCol_ResizeGripHovered] = Hovered(colors[ImGuiCol_ResizeGrip]);
		colors[ImGuiCol_ResizeGripActive] = Active(colors[ImGuiCol_ResizeGrip]);
		colors[ImGuiCol_Tab] = GetColor(MainColor, Alpha60);
		colors[ImGuiCol_TabHovered] = Hovered(colors[ImGuiCol_Tab]);
		colors[ImGuiCol_TabActive] = Active(colors[ImGuiCol_Tab]);
		colors[ImGuiCol_TabUnfocused] = colors[ImGuiCol_Tab];
		colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_TabActive];
		colors[ImGuiCol_DockingPreview] = Darken(colors[ImGuiCol_HeaderActive], 0.2f);
		colors[ImGuiCol_DockingEmptyBg] = Darken(colors[ImGuiCol_HeaderActive], 0.6f);
		colors[ImGuiCol_PlotLines] = GetColor(HighlightColor);
		colors[ImGuiCol_PlotLinesHovered] = Hovered(colors[ImGuiCol_PlotLines]);
		colors[ImGuiCol_PlotHistogram] = GetColor(HighlightColor);
		colors[ImGuiCol_PlotHistogramHovered] = Hovered(colors[ImGuiCol_PlotHistogram]);
		colors[ImGuiCol_TextSelectedBg] = GetColor(HighlightColor, Alpha40);
		colors[ImGuiCol_DragDropTarget] = GetColor(HighlightColor, Alpha80);;
		colors[ImGuiCol_NavHighlight] = GetColor(White);
		colors[ImGuiCol_NavWindowingHighlight] = GetColor(White, Alpha80);
		colors[ImGuiCol_NavWindowingDimBg] = GetColor(White, Alpha20);
		colors[ImGuiCol_ModalWindowDimBg] = GetColor(Black, Alpha60);

		ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_Right;

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(8.00f, 8.00f);
		style.FramePadding = ImVec2(5.00f, 2.00f);
		style.CellPadding = ImVec2(6.00f, 6.00f);
		style.ItemSpacing = ImVec2(6.00f, 6.00f);
		style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing = 25;
		style.ScrollbarSize = 15;
		style.GrabMinSize = 10;
		style.WindowBorderSize = BorderWidth;
		style.ChildBorderSize = BorderWidth;
		style.PopupBorderSize = BorderWidth;
		style.FrameBorderSize = BorderWidth;
		style.TabBorderSize = BorderWidth;
		style.WindowRounding = 2;
		style.ChildRounding = 2;
		style.FrameRounding = 2;
		style.PopupRounding = 2;
		style.ScrollbarRounding = 9;
		style.GrabRounding = 3;
		style.LogSliderDeadzone = 4;
		style.TabRounding = 4;
	}

	static void ApplyDark()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
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
		colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
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
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
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
		style.IndentSpacing = 25;
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
	}
};
