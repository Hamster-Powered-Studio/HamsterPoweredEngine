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

    void UI_Toolbar();
    void OnScenePlay();
    void OnSceneStop();

    
    UIViewport* uiviewport;
    UIHierarchy* hier;
    ImGuiViewport* viewport;
    ImGuiIO* io;
    EditorCamera m_EditorCamera;
    
    GameWindow& Game;

    std::vector<ImGuiElement*> UIElements;

    static inline Actor selection;
    
    template<typename T, typename ...Args>
    T* Construct(Args && ...args);

    void Destroy(ImGuiElement* element);

    enum class SceneState
    {
        Edit = 0, Play = 1
    };

    sf::Texture m_iconPlay;
    sf::Texture m_iconStop;
    SceneState m_SceneState = SceneState::Edit;
};
