#pragma once
#include "ImGuiElement.h"
#include "UIHierarchy.h"
#include "Engine/Actor.h"

class UIActorDetails :
    public ImGuiElement
{
public:
    UIActorDetails();

    void OnImGuiRender() override;


    std::string label = "Details";
    std::string textInput;

    void SetContext(UIHierarchy* context) { m_Context = context; }

    UIHierarchy* m_Context;

};

