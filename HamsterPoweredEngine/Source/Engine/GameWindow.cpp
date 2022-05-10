
#include "GameWindow.h"
#include <iostream>
#include "Actor.h"
#include "Global.h"
#include "UI/UIHierarchy.h"
#include "Engine.h"
#include <SFML/OpenGL.hpp>
#include "Utils/EditorColorScheme.h"
#include <windows.h>
#include "SceneSerializer.h"
#include "Utils/PlatformUtils.h"
#include "Engine/EditorLayer.h"
#include "Scripts/CameraController.h"
#include "Scripts/TileMapCollisionScript.h"
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
    
    LuaEngine::lua.open_libraries(sol::lib::base);
    global::Game = this;

    //Create the main instance window
    window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "Hamster Powered Engine");
    //::ShowWindow(window->getSystemHandle(), SW_MAXIMIZE);
    window->setVerticalSyncEnabled(true);
    auto image = sf::Image{};
    if (image.loadFromFile("resources/icon.png"))
    {
        window->setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());
    }

    
    ImGui::SFML::Init(*window, true);

    //Load editor functions and utils
    if (EDITOR) editor = new EditorLayer(*this);
    
    Renderer::GetInstance();
    //Renderer::Resize(window->getSize().x, window->getSize().y);

    
    NewEmptyScene();
    if (!EDITOR)
    {
        SceneSerializer serializer(*currentScene);
        serializer.Deserialize("assets/scenes/ForestWithScript.hpl");
        ResizeGameView();
    }
    
    
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
            if (EDITOR) editor->m_EditorCamera.OnEvent(event);
            if (event.type == sf::Event::Closed)
                window->close();

            if (event.type == sf::Event::Resized) {
                if(!EDITOR)
                    ResizeGameView();
            }
        }
        global::deltaTime = global::deltaClock.getElapsedTime().asSeconds();
        
        ImGui::SFML::Update(*window, global::deltaClock.restart());


        if (editor)
        {
            editor->OnUpdate();
            //Renderer::BeginDraw();
            //currentScene->OnUpdateRuntime(global::deltaClock.getElapsedTime());
            //Renderer::EndDraw();
        }
        else
        {
            window->clear();

            Renderer::BeginDraw();
            currentScene->OnUpdateRuntime(global::deltaTime);
            Renderer::EndDraw();
        }


        sf::Sprite output;
        output.setTexture(Renderer::GetView()->getTexture(), true);
        //output.setOrigin(output.getTexture()->getSize().x / 2, output.getTexture()->getSize().y / 2);

        if (!EDITOR)
        {
            window->setView(sf::View((sf::FloatRect)output.getTextureRect()));
            window->draw(output);
        }
            
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
    if (EDITOR)
    {
        ImVec2 view = ImGui::GetContentRegionAvail();
        Renderer::Resize(view.x, view.y);
        editor->m_EditorCamera.Resize(view.x, view.y);
        if (editor->m_SceneState == EditorLayer::SceneState::Play)
        {
            if (currentScene->mainCamera)
            {
                currentScene->mainCamera->Camera.setSize(view.x, view.y);
            }
        }
        std::cout << "resized";
    }
    else
    {
        sf::Vector2u view = window->getSize();
        Renderer::Resize(view.x, view.y);
        if (currentScene->mainCamera)
        {
            currentScene->mainCamera->Camera.setSize(view.x, view.y);
            
        }
    }
    
}







void GameWindow::NewEmptyScene()
{
    delete currentScene;
    currentScene = new Scene();
    Actor Cam = currentScene->CreateActor("Camera");
    for (int i = 0; i < 10; i++)
        currentScene->CreateActor("Lua").AddComponent<LuaScriptComponent>().Bind<ScriptableLuaActor>();
    
    Cam.AddComponent<CameraComponent>().SetPrimary(false);
    
    Cam.AddComponent<NativeScriptComponent>().Bind<CameraController>();
    Cam.AddComponent<LuaScriptComponent>().Bind<ScriptableLuaActor>();
    if (editor && editor->hier)
    { 
        editor->hier->SetContext(*currentScene);
    }
}

