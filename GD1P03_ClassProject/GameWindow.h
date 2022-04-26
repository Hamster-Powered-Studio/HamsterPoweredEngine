#pragma once

#include "Actor.h"
#include "SFML/Graphics.hpp"
#include "imgui.h"
#include "imgui-SFML.h"
#include "Level.h"
#include "ImGuiElement.h"
#include "Scene.h"

class UIHierarchy;

class GameWindow
{
    

public:
    sf::RenderWindow* window;
    GameWindow();
    ~GameWindow();

    void Begin();
    void WindowLoop();

    void SetActiveView();

    Scene* currentScene;

    ImGuiIO* io;

    void ResizeGameView();
    
    void ModernDarkTheme();

    Actor* selectedActor = nullptr;

    ImGuiViewport* viewport;

    template<typename T, typename... Args>
    T* Construct(Args&&... args);

    void Destroy(ImGuiElement* Element);

    void RenderUI();
    std::vector<ImGuiElement*> UIElements;

    UIHierarchy* hier;
private:

    
};
