#pragma once
#include "ImGuiElement.h"
#include "Engine/Scene.h"
#include "Engine/Global.h"
#include "Engine/Actor.h"


class UIHierarchy :
    public ImGuiElement
{
public:
    UIHierarchy() = default;
    UIHierarchy(Scene& scene);

    void SetContext(Scene& context);

    void OnImGuiRender() override;

    //void RenderNodeTree(Actor* i, int& index);

    int selectedIndex = 0;

    
    //friend class UIActorDetails;

    std::string label = "Hierarchy";

private:
    friend class GameWindow;
    friend class UIViewport;
    Scene* m_Context;
    void DrawActorNode(Actor actor);
    void DrawComponents(Actor& selection);
    Actor m_SelectionContext;

};

