#pragma once
#include "GameWindow.h"
#include <functional>
#include <iostream>
#include "Actor.h"
#include "imgui_stdlib.h"
#include "Global.h"
#include "UI/UIViewport.h"
#include "UI/UIHierarchy.h"
#include "Engine.h"
#include <SFML/OpenGL.hpp>
#include "imgui_impl_opengl2.h"
#include "Utils/EditorColorScheme.h"
#include "Utils/EditorColorPicker.h"
#include <windows.h>
#include "SceneSerializer.h"
#include "Utils/PlatformUtils.h"
#include "Engine/EditorLayer.h"
#include "Scripts/CameraController.h"
#include "Scripts/CloseGameCollider.h"
#include <sol/sol.hpp>

#include "ScriptableLuaActor.h"

void doSomething()
{
    std::cout << "Did something!";
}

void some_function() {
    std::cout << "some function!" << std::endl;
}

GameWindow::GameWindow()
{
    
    global::Game = this;

    
    
    //sol::state lua;
    
    //lua.open_libraries(sol::lib::base);

    
    window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "Hamster Powered Engine");
    ::ShowWindow(window->getSystemHandle(), SW_MAXIMIZE);
    
    auto image = sf::Image{};
    if (image.loadFromFile("resources/icon.png"))
    {
        window->setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());
    }
    
    ImGui::SFML::Init(*window, true);
    if (EDITOR) editor = new EditorLayer(*this);
    
    Renderer::GetInstance();
    Renderer::Resize(window->getSize().x, window->getSize().y);
    m_EditorCamera.m_ViewportSize = sf::Vector2u(window->getSize().x, window->getSize().y);
    
    NewEmptyScene();
    WindowLoop();
}

GameWindow::~GameWindow()
{
    delete currentScene;
    delete window;
}

void GameWindow::WindowLoop()
{


    while (window->isOpen())
    {
        bool newLevel = false;
        sf::Event event{};
        
        while (window->pollEvent(event))
        {
            global::event = &event;
            ImGui::SFML::ProcessEvent(event);
            m_EditorCamera.OnEvent(event);
            if (event.type == sf::Event::Closed)
                window->close();

            if (event.type == sf::Event::Resized) {
                if(!EDITOR)
                    Renderer::Resize(window->getSize().x, window->getSize().y);
            }
        }
        global::deltaTime = global::deltaClock.getElapsedTime();
        ImGui::SFML::Update(*window, global::deltaClock.restart());


        if (editor)
        {
            editor->OnUpdate();
            //Renderer::BeginDraw();
            currentScene->OnUpdateRuntime(global::deltaClock.getElapsedTime());
            //Renderer::EndDraw();
        }
        else
        {
            window->clear();

            Renderer::BeginDraw();
            currentScene->OnUpdateRuntime(global::deltaClock.getElapsedTime());
            Renderer::EndDraw();
        }


        sf::Sprite output;
        output.setTexture(Renderer::GetView()->getTexture(), true);

        if (!EDITOR)
        window->draw(output);
        else
        {
            ImGui::SFML::Render(*window);
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
        

        window->display();

    }
}



void GameWindow::ResizeGameView()
{
    if (!NDEBUG)
    {
        ImVec2 view = ImGui::GetContentRegionAvail();
        Renderer::Resize(view.x, view.y);
        m_EditorCamera.Resize(view.x, view.y);
    }
    
}







void GameWindow::NewEmptyScene()
{
    delete currentScene;
    currentScene = new Scene();
    Actor Cam = currentScene->CreateActor("Camera");
    for (int i = 0; i < 10; i++)
        currentScene->CreateActor("Lua").AddComponent<LuaScriptComponent>().Bind<ScriptableLuaActor>();
    
    Cam.AddComponent<CameraComponent>().SetPrimary(true);
    
    Cam.AddComponent<NativeScriptComponent>().Bind<CameraController>();
    Cam.AddComponent<LuaScriptComponent>().Bind<ScriptableLuaActor>();
    if (editor && editor->hier)
    { 
        editor->hier->SetContext(*currentScene);
    }
}

