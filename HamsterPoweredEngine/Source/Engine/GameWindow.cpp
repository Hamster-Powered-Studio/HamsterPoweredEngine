
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
#include "ScriptableLuaActor.h"


void doSomething()
{
    std::cout << "Did something!";
}

void some_function() {
    std::cout << "some function!" << std::endl;
}


void GameWindow::OpenLevel(std::string path)
{
    levelToOpen = path;
}

GameWindow::GameWindow()
{
    
    LuaEngine::lua.open_libraries(sol::lib::base);
    global::Game = this;

    //Create the main instance window
    window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "Hamster Powered Engine");
    //::ShowWindow(window->getSystemHandle(), SW_MAXIMIZE);
    window->setFramerateLimit(60);
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
        serializer.Deserialize("assets/scenes/Menu.hpl");
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
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Key::Tab)
                {
                    DebugWindow = !DebugWindow;
                }
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

        sf::Texture heart;
        heart.loadFromFile("assets/sprites/heart.png");
        sf::Vector2i root = window->getPosition();
        for (int i = 0; i < GameState.Lives; i++)
        {
            sf::Sprite spr = sf::Sprite(heart);
            spr.setPosition(Renderer::GetView()->getSize().x / 2 + i*20 - 29, Renderer::GetView()->getSize().y / 2 - 50);
            window->draw(spr);
        }

        if (GameState.Lives <= 0)
        {
            levelToOpen = "assets/scenes/Menu.hpl";
            GameState.Lives = 3;
        }
        
        if (levelToOpen != "")
        {
            NewEmptyScene();
            SceneSerializer serializer(*currentScene);
            serializer.Deserialize(levelToOpen);
            levelToOpen = "";
        }

        
        ImGui::SFML::Render(*window);
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

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
    //Actor Cam = currentScene->CreateActor("Camera");
    
    //Cam.AddComponent<CameraComponent>().SetPrimary(false);
    
    //Cam.AddComponent<NativeScriptComponent>().Bind<CameraController>();
    //Cam.AddComponent<LuaScriptComponent>().Bind<ScriptableLuaActor>();
    if (editor && editor->hier)
    { 
        editor->hier->SetContext(*currentScene);
    }
    //currentScene->mainCamera = &Cam.GetComponent<CameraComponent>();
    //ResizeGameView();
}

