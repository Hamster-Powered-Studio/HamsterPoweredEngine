#pragma once
#include "ImGuiElement.h"
//#include "Engine/EditorLayer.h"

class EditorLayer;

class UIViewport :
    public ImGuiElement
{
public:
    UIViewport();
    UIViewport(EditorLayer* editor);
    ~UIViewport() {}
    void OnImGuiRender() override;
    EditorLayer* Editor;
    ImVec2 size;
    bool focused;
    std::string label = "Viewport";
};

