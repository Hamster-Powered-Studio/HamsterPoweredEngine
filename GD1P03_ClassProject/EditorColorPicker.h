#pragma once
#include "EditorColorScheme.h"
#include "ImGuiElement.h"

class EditorColorPicker : public ImGuiElement
{
public:
	EditorColorPicker();
	void OnImGuiRender() override;
	//float bg[4]{ 0, 0, 0, 0 };

	EditorColorScheme::Theme theme;
	std::string label = "Editor Theme";
};

