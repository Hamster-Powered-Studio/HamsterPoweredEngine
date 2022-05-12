#pragma once

#include "Actor.h"
#include "SFML/Graphics.hpp"
#include "imgui.h"
#include "imgui-SFML.h"
#include "Scene.h"
#include "Engine/GameState.h"


class EditorLayer;

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

    EditorLayer* editor;
    void NewEmptyScene();
    Scene* currentScene;

    ImGuiIO* io;

    void ResizeGameView();
    

    Actor* selectedActor = nullptr;

    bool DebugWindow = false;
    
    void OpenLevel(std::string path);
    GameState GameState;
    
private:
    std::string levelToOpen = "";
    //static inline GameWindow* GameWindow::GameWindow_m = nullptr;
    GameWindow();

};
