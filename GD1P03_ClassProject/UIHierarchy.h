#pragma once
#include "ImGuiElement.h"
#include "Scene.h"
#include "Global.h"
#include "Actor.h"


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

    
    friend class UIActorDetails;

    std::string label = "Hierarchy";

private:
    friend class GameWindow;
    Scene* m_Context;
    void DrawActorNode(Actor& actor);
    void DrawComponents(Actor& selection);
    Actor m_SelectionContext;

};

