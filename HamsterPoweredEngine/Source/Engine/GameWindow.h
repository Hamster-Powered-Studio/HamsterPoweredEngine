#pragma once

#include "Actor.h"
#include "SFML/Graphics.hpp"
#include "imgui.h"
#include "imgui-SFML.h"
#include "UI/ImGuiElement.h"
#include "Scene.h"

class UIHierarchy;

class GameWindow
{
    

public:
    sf::RenderWindow* window;

    ~GameWindow();
    GameWindow(GameWindow const&) = delete;
    GameWindow& operator=(GameWindow const&) = delete;

    static std::shared_ptr<GameWindow> instance()
    {
        static std::shared_ptr<GameWindow> s{new GameWindow};
        return s;
    }
    
    void Begin();
    void WindowLoop();

    void SetActiveView();

    void NewEmptyScene();
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
    //static inline GameWindow* GameWindow::GameWindow_m = nullptr;
    GameWindow();
};
