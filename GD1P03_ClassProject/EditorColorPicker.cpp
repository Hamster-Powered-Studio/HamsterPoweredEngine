#include "EditorColorPicker.h"
#include <imgui_internal.h>
#include <iostream>

EditorColorPicker::EditorColorPicker()
{
	CreateLabelID(label);
	theme = EditorColorScheme::GetColors();
}

void EditorColorPicker::OnImGuiRender()
{
	
	ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs;

	ImGui::Begin(label.c_str(), &open);
	bool value_changed = false;

	std::string hex = std::to_string(theme.BackGroundColor);
	if (ImGui::InputText("Background", &hex))
	{
		hex = "0x" + hex;
		theme.BackGroundColor = atoi(hex.c_str());
	}
	



	if (true)
	{

		EditorColorScheme::SetColors(theme.BackGroundColor, theme.TextColor, theme.MainColor, theme.MainAccentColor, theme.HighlightColor);
		EditorColorScheme::ApplyTheme();
	}


	ImGui::End();

	

}
