#pragma once
#include "ImGuiElement.h"

class UIViewport :
    public ImGuiElement
{
public:
    UIViewport();
    void OnImGuiRender() override;

    ImVec2 size;

    std::string label = "Viewport";
};

