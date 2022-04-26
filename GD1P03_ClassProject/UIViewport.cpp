#include "UIViewport.h"
#include "Global.h"
#include "Renderer.h"
#include <iostream>

UIViewport::UIViewport()
{
	CreateLabelID(label);
	
	
}

void UIViewport::OnImGuiRender()
{

	ImGuiElement::OnImGuiRender();

	ImGui::Begin(label.c_str(), &open);

	if (ImGui::GetWindowSize().x != size.x || ImGui::GetWindowSize().y != size.y)
	{
		global::Game->ResizeGameView();
	}

	size = ImGui::GetWindowSize();

	ImGui::Image(*Renderer::GetView());

	ImGui::End();

	if (!open) global::Game->Destroy(this);

}