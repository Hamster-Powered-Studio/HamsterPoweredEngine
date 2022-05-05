#pragma once

#include "UI/UIHierarchy.h"
#include "UI/UIViewport.h"

class GameWindow;

class EditorLayer
{
public:
    EditorLayer(GameWindow& instance);

    void OnUpdate();
    void ConstructUI();
    void RenderUI();

    UIViewport* uiviewport;
    UIHierarchy* hier;
    ImGuiViewport* viewport;
    ImGuiIO* io;

    GameWindow& Game;

    std::vector<ImGuiElement*> UIElements;

    static inline Actor selection;
    
    template<typename T, typename ...Args>
    T* Construct(Args && ...args);

    void Destroy(ImGuiElement* element);

};
