#pragma once
#include "GameWindow.h"
#include <functional>
#include <iostream>
#include "Actor.h"
#include "imgui_stdlib.h"
#include "Global.h"
#include "UI/UIViewport.h"
#include "UI/UIActorDetails.h"
#include "UI/UIHierarchy.h"
#include "Engine.h"
#include <SFML/OpenGL.hpp>
#include "imgui_impl_opengl2.h"
#include "Utils/EditorColorScheme.h"
#include "Utils/EditorColorPicker.h"
#include <windows.h>
#include "SceneSerializer.h"
#include "Utils/PlatformUtils.h"

GameWindow::GameWindow()
{
    //window = ;
    global::Game = this;
    window = new sf::RenderWindow(sf::VideoMode(1920, 1080), "Hamster Powered Engine");
    ::ShowWindow(window->getSystemHandle(), SW_MAXIMIZE);
    Renderer::GetInstance();
    Renderer::Resize(window->getSize().x, window->getSize().y);
    Begin();
}

GameWindow::~GameWindow()
{
    //delete currentLevel;
    delete currentScene;
    delete window;
}

void GameWindow::Begin()
{
    //LoadLevel("test");
    NewEmptyScene();

    Construct<UIViewport>();
    hier = Construct<UIHierarchy>(*currentScene);
    
    //Construct<EditorColorPicker>();
    //Construct<UIActorDetails>()->SetContext(hier);

    std::cout << "Game Window made!";
    
    //ImGui
    ImGui::SFML::Init(*window, true);
    
    EditorColorScheme::ApplyFonts();
    //EditorColorScheme::SetColors(0x131112FF, 0xFFFFFFFF, 0x673B70FF, 0x2C2139FF, 0xFB4B4EFF);
    //EditorColorScheme::ApplyTheme();
    EditorColorScheme::ApplyDark();

    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io->ConfigDockingTransparentPayload = true;
    io->ConfigDragClickToInputText = true;
    io->BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;


    WindowLoop();
}

void GameWindow::WindowLoop()
{
#ifdef IMGUI_HAS_VIEWPORT
    viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    
#else 
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
#endif

    while (window->isOpen())
    {
        bool newLevel = false;
        sf::Event event{};
        
        while (window->pollEvent(event))
        {
            global::event = &event;
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window->close();

            if (event.type == sf::Event::Resized) {
                if(NDEBUG)
                    Renderer::Resize(window->getSize().x, window->getSize().y);
            }
        }
        global::deltaTime = global::deltaClock.getElapsedTime();
        ImGui::SFML::Update(*window, global::deltaClock.restart());


        if (EDITOR)
        {
            ImGuiStyle& style = ImGui::GetStyle();
            style.WindowMinSize.x = 370.f;
            ImGui::DockSpaceOverViewport(viewport);
            style.WindowMinSize.x = 32;
            RenderUI();
       
        }
        else window->clear();

        Renderer::BeginDraw();
        currentScene->Update(global::deltaClock.getElapsedTime());
        Renderer::EndDraw();



        sf::Sprite output;
        output.setTexture(Renderer::GetView()->getTexture(), true);

        if (!EDITOR)
        window->draw(output);

        ImGui::SFML::Render(*window);
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        window->display();

    }
}



void GameWindow::ResizeGameView()
{
    if (!NDEBUG)
    {
        ImVec2 view = ImGui::GetContentRegionAvail();
        Renderer::Resize(view.x, view.y);
    }
    
}



template<typename T, typename... Args>
T* GameWindow::Construct(Args&&... args)
{
    T* element = new T((args)...);
    ImGuiElement* el = dynamic_cast<ImGuiElement*>(element);
    UIElements.push_back(el);
    return element;
}

void GameWindow::Destroy(ImGuiElement* Element)
{
    std::vector<ImGuiElement*>::iterator it;

    it = std::find(UIElements.begin(), UIElements.end(), Element);
    UIElements.erase(it);

    delete Element;
    Element = nullptr;

}

void GameWindow::NewEmptyScene()
{
    delete currentScene;
    currentScene = new Scene();
    Actor Cam = currentScene->CreateActor("Camera");
    Cam.AddComponent<CameraComponent>().SetPrimary(true);
    Cam.AddComponent<InputComponent>().Active = true;
    if (hier)
    {
        hier->SetContext(*currentScene);
    }
}

void GameWindow::RenderUI()
{

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New", "Ctrl+N"))
        {
            NewEmptyScene();
        }

        if (ImGui::MenuItem("Open...", "Ctrl+O")) {
            std::string filepath = FileDialogs::OpenFile(window->getSystemHandle(), "Hamster Powered Level (*.hpl)\0*.hpl\0");
            if (!filepath.empty())
            {
                delete currentScene;
                currentScene = new Scene();
                if (hier)
                {
                    hier->SetContext(*currentScene);
                }
                
                SceneSerializer serializer(*currentScene);
                serializer.Deserialize(filepath);
            }
            
        }

        if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
            std::string filepath = FileDialogs::SaveFile(window->getSystemHandle(), "Hamster Powered Level (*.hpl)\0*.hpl\0");
            if (!filepath.empty())
            {
                SceneSerializer serializer(*currentScene);
                serializer.Serialize(filepath);
            }

            
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Exit")) exit(0);
        ImGui::EndMenu();
    }


    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Nothing")) {};
        ImGui::EndMenu();
    }


    if (ImGui::BeginMenu("Windows"))
    {
        if (ImGui::MenuItem("Viewport")) { Construct<UIViewport>(); }
        if (ImGui::MenuItem("Details")) { Construct<UIActorDetails>(); }
        if (ImGui::MenuItem("Hierarchy")) { Construct<UIHierarchy>(*currentScene); }
        if (ImGui::MenuItem("Content Browser")) {};
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();

    float fps = 1.f / global::deltaTime.asSeconds();
    ImGui::Begin("Stats");
    ImGui::Text("FPS: %f", fps);
    ImGui::Text("Render Resolution: %i x %i", Renderer::GetView()->getSize().x, Renderer::GetView()->getSize().y);
    ImGui::End();

    for (unsigned int i = 0; i < UIElements.size(); i++)
    {

            UIElements[i]->OnImGuiRender();

    }
}
