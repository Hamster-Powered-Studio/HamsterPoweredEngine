#include "ImGuiElement.h"
#include <algorithm>
#include <iostream>

ImGuiElement::ImGuiElement()
{
	elements++;
	//NumberLabel(label);

}


ImGuiElement::~ImGuiElement()
{
	elements--;
}

void ImGuiElement::CreateLabelID(std::string &label)
{
	label = label + "###" + label + "_" + std::to_string(ImGuiElement::elements);
}

void ImGuiElement::OnImGuiRender()
{
	//ImGui::SetNextWindowSizeConstraints(ImVec2(100.0f, 300.0f), ImVec2(INFINITY, INFINITY));



}
