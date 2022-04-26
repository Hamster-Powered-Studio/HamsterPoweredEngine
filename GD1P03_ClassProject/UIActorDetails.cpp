#include "UIActorDetails.h"
#include "imgui_stdlib.h"
#include "Global.h"
#include "Components.h"

UIActorDetails::UIActorDetails()
{
    CreateLabelID(label);
}


void UIActorDetails::OnImGuiRender()
{
    

    ImGuiElement::OnImGuiRender();

    ImGui::Begin(label.c_str());
 
    if (m_Context && m_Context->m_SelectionContext)
    {
        auto& actor = m_Context->m_SelectionContext;
        ImGui::Text(actor.GetComponent<TagComponent>().Tag.c_str());
        }
        
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        //selectedActor->DisplayExposedVars();

        

    ImGui::End();

    if (!open) this->ImGuiElement::~ImGuiElement();
}


