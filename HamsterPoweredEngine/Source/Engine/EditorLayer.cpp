#include "EditorLayer.h"
#include "Utils/EditorColorScheme.h"
#include "Utils/PlatformUtils.h"
#include "SceneSerializer.h"
#include "Renderer.h"
#include "Components/Components.h"
//#include "Engine/GameWindow.h"

EditorLayer::EditorLayer(GameWindow& instance) : Game(instance)
{
    m_EditorCamera.m_ViewportSize = sf::Vector2u(Game.window->getSize().x, Game.window->getSize().y);
    m_iconPlay.loadFromFile("resources/playbutton.png");
    m_iconStop.loadFromFile("resources/stopbutton.png");
    ConstructUI();
}

void EditorLayer::ConstructUI()
{
    uiviewport = Construct<UIViewport>();
    uiviewport->Editor = this;
    //uiviewport = new UIViewport(this);
    //UIElements.push_back(uiviewport);
    hier = Construct<UIHierarchy>(*Game.currentScene);
    
    //Construct<EditorColorPicker>();
    //Construct<UIActorDetails>()->SetContext(hier);

    //ImGui
    

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


#ifdef IMGUI_HAS_VIEWPORT
    viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

#else 
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
#endif

}

template<typename T, typename... Args>
T* EditorLayer::Construct(Args&&... args)
{
    T* element = new T((args)...);
    ImGuiElement* el = dynamic_cast<ImGuiElement*>(element);
    UIElements.push_back(el);
    return element;
}

void EditorLayer::RenderUI()
{

    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New", "Ctrl+N"))
        {
            Game.NewEmptyScene();
        }

        if (ImGui::MenuItem("Open...", "Ctrl+O")) {
            std::string filepath = FileDialogs::OpenFile(Game.window->getSystemHandle(), "Hamster Powered Level (*.hpl)\0*.hpl\0");
            if (!filepath.empty())
            {
                delete Game.currentScene;
                Game.currentScene = new Scene();
                if (hier)
                {
                    hier->SetContext(*Game.currentScene);
                }

                SceneSerializer serializer(*Game.currentScene);
                serializer.Deserialize(filepath);
            }

        }

        if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
            std::string filepath = FileDialogs::SaveFile(Game.window->getSystemHandle(), "Hamster Powered Level (*.hpl)\0*.hpl\0");
            if (!filepath.empty())
            {
                SceneSerializer serializer(*Game.currentScene);
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
        //if (ImGui::MenuItem("Details")) { Construct<UIActorDetails>(); }
        if (ImGui::MenuItem("Hierarchy")) { Construct<UIHierarchy>(*Game.currentScene); }
        if (ImGui::MenuItem("Content Browser")) {};
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();

    float fps = 1.f / global::deltaTime;
    ImGui::Begin("Stats");
    ImGui::Text("FPS: %f", fps);
    ImGui::Text("Render Resolution: %i x %i", Renderer::GetView()->getSize().x, Renderer::GetView()->getSize().y);
    ImGui::End();

    UI_Toolbar();
    
    for (unsigned int i = 0; i < UIElements.size(); i++)
    {

        UIElements[i]->OnImGuiRender();

    }
}

void EditorLayer::UI_Toolbar()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    auto& colors = ImGui::GetStyle().Colors;
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(colors[ImGuiCol_ButtonHovered].x, colors[ImGuiCol_ButtonHovered].y, colors[ImGuiCol_ButtonHovered].z, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(colors[ImGuiCol_ButtonActive].x, colors[ImGuiCol_ButtonActive].y, colors[ImGuiCol_ButtonActive].z, 0.7f));
    
    sf::Texture& icon = m_SceneState == SceneState::Edit ? m_iconPlay : m_iconStop;
    icon.setSmooth(true);
    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    float size = ImGui::GetContentRegionAvail().y - 4.0f;
    ImGui::SameLine((ImGui::GetContentRegionMax().x * 0.5) - (size * 0.5f));
        if (ImGui::ImageButton(icon, ImVec2(size, size)))
        {
            if (m_SceneState == SceneState::Edit)
            {
                OnScenePlay();
            }
            else if (m_SceneState == SceneState::Play)
            {
                OnSceneStop();
            }
        }
    ImGui::End();
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);
}

void EditorLayer::OnScenePlay()
{
    m_SceneState = SceneState::Play;
    Game.currentScene->Reg().view<LuaScriptComponent>().each([=](LuaScriptComponent& lsc) { lsc.Instance->ReloadScripts(); });
}

void EditorLayer::OnSceneStop()
{
    m_SceneState = SceneState::Edit;
}

void EditorLayer::OnUpdate()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowMinSize.x = 370.f;
    ImGui::DockSpaceOverViewport(viewport);
    style.WindowMinSize.x = 32;
    RenderUI();

    if (uiviewport)
    {
        if (uiviewport->focused)
            m_EditorCamera.OnUpdate(global::deltaTime);
    }

    
    Renderer::BeginDraw();
    switch (m_SceneState)
    {
    case SceneState::Edit:
        {
            Game.currentScene->OnUpdateEditor(global::deltaTime, m_EditorCamera);
            break;
        }
    case SceneState::Play:
        {
            //Game.currentScene->OnUpdateEditor(global::deltaClock.getElapsedTime(), m_EditorCamera);
            Game.currentScene->OnUpdateRuntime(global::deltaTime);
            break;
        }
    }
    
    Renderer::EndDraw();
   }


void EditorLayer::Destroy(ImGuiElement* Element)
{
    std::vector<ImGuiElement*>::iterator it;

    it = std::find(UIElements.begin(), UIElements.end(), Element);
    UIElements.erase(it);

    delete Element;
    Element = nullptr;

}