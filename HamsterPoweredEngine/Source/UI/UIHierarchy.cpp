#include "UIHierarchy.h"
#include "Engine/Actor.h"
#include "entt.hpp"
#include "Components/Components.h"
#include "imgui_internal.h"
#include <iostream>
#include "Engine/Scene.h"
#include "Engine/EditorLayer.h"
#include "Scripts/CloseGameCollider.h"
#include "Scripts/CameraController.h"

UIHierarchy::UIHierarchy(Scene& scene) : m_Context(&scene)
{
}

void UIHierarchy::SetContext(Scene& context)
{
    m_Context = &context;
    EditorLayer::selection = {};
}

void UIHierarchy::OnImGuiRender()
{
    ImGuiElement::OnImGuiRender();
    
    ImGui::Begin(label.c_str(), &open);
    int index = 0;
    DragDone = false;
    
    if (m_Context)
        {

        m_Context->Reg().each([&](auto actorID)
            {
                Actor actor{ actorID, m_Context };
                auto& relations =  actor.GetComponent<RelationshipComponent>();
                if (!relations.Parent)
                    DrawActorNode(actor);
            });
        }

    if (ImGui::IsMouseReleased(0) && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && ImGui::GetDragDropPayload() && !DragDone)
    {
        const ImGuiPayload* payload = ImGui::GetDragDropPayload();
        if (payload)
        {
            DragDone = true;
            if(payload->IsDataType("HIERARCHY_ACTOR"))
            {
                Actor receivedActor = *(Actor*)payload->Data;
                auto& payloadRelations = receivedActor.GetComponent<RelationshipComponent>();
                if (payloadRelations.Parent) //if the dragged object has a parent
                    {
                    auto& otherParentsChildren = payloadRelations.Parent.GetComponent<RelationshipComponent>().children; //get that parent's children
                    otherParentsChildren.erase(std::remove(otherParentsChildren.begin(), otherParentsChildren.end(), receivedActor)); // remove itself from that parent's child list
                    payloadRelations.Parent = {};
                    }
            }
        }
    }
    
    
    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
    {
        EditorLayer::selection = {};
    }
    
    
    if (ImGui::BeginPopupContextWindow("Create", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
    {
        if (ImGui::MenuItem("Create Actor"))
            m_Context->CreateActor("New Actor");
        ImGui::EndPopup();
    }

    

    ImGui::End();


    ImGui::Begin("Details");
    if (EditorLayer::selection)
    {
        DrawComponents(EditorLayer::selection);
    }
    ImGui::End();
}

bool isEqual(Actor& actor, Actor& actor2){
    return (actor == actor2);
}

void UIHierarchy::DrawActorNode(Actor actor)
{
    auto& tag = actor.GetComponent<TagComponent>().Tag;
    auto& relations =  actor.GetComponent<RelationshipComponent>();
    ImGuiTreeNodeFlags flags = { ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick };
    
    if (relations.children.size() == 0) flags |= ImGuiTreeNodeFlags_Leaf;
    if (EditorLayer::selection == actor) flags |= ImGuiTreeNodeFlags_Selected;

    
    bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)actor, flags, tag.c_str());

    
    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("HIERARCHY_ACTOR", &EditorLayer::selection, sizeof(Actor));
        ImGui::EndDragDropSource();
    }   
    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload* payload =  ImGui::AcceptDragDropPayload("HIERARCHY_ACTOR");
        if (payload)
        {
            Actor receivedActor = *(Actor*)payload->Data;
            
            if (receivedActor.HasComponent<RelationshipComponent>())
            {
                DragDone = true;
                auto& payloadRelations = receivedActor.GetComponent<RelationshipComponent>();
                if (payloadRelations.Parent) //if the dragged object has a parent
                {
                    auto& otherParentsChildren = payloadRelations.Parent.GetComponent<RelationshipComponent>().children; //get that parent's children
                    otherParentsChildren.erase(std::remove(otherParentsChildren.begin(), otherParentsChildren.end(), receivedActor)); // remove itself from that parent's child list
                    
                }
                if (payloadRelations.Parent != actor)
                {
                    if (!std::any_of(relations.children.begin(), relations.children.end(), [&](const Actor& other) {return receivedActor == other;})) // if payload is not already a child
                        {
                            relations.children.push_back(receivedActor); //add payload as a child
                            payloadRelations.Parent = actor;  //set payloads parent to be this
                        }
                }
                else
                {
                    payloadRelations.Parent = {};
                }
                
                
                
                std::cout << "Done!";
            }

        }
        ImGui::EndDragDropTarget();
    }
    


    bool actorDeleted = false;
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Destroy Actor"))
            actorDeleted = true;

        ImGui::EndPopup();
    }

    if (ImGui::IsItemClicked())
    {
        EditorLayer::selection = actor;
    }

    if (opened)
    {
        ImGui::TreePop();
        ImGui::Indent();
        
        for (auto child : relations.children)
        {
            DrawActorNode(child);
        }
        ImGui::Unindent();
        
    }
        
        if (actorDeleted)
        {
            m_Context->DestroyActor(EditorLayer::selection);
            if (EditorLayer::selection == actor)
                EditorLayer::selection = {};
        }
}

void Space()
{
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();
}

static bool DrawVec2Control(const std::string& label, sf::Vector2f& values, float resetValue = 0.0f, float speed = 1.f, float columnWidth = 100.0f)
{
    bool changed = false;
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());
    ImGui::Columns(2);

    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.7f, 0.0f, 0.1f, 1.0f });

    ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize))
        values.x = resetValue;
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    ImGui::SameLine();
    if (ImGui::DragFloat("##X", &values.x, speed)) changed = true;
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.4f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.6f, 0.2f, 1.0f });

    ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize))
        values.y = resetValue;
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    ImGui::SameLine();
    if (ImGui::DragFloat("##Y", &values.y, speed)) changed = true;
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);
    ImGui::PopID();

    return changed;
}

static bool DrawFloatControl(const std::string& label, float& value, float resetValue = 0.0f, float speed = 1.f, float columnWidth = 100.0f)
{
    bool changed = false;
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());
    ImGui::Columns(2);

    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();


    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth() + buttonSize.y-3.f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.7f, 0.0f, 0.1f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize))
        value = resetValue;
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    ImGui::SameLine();
    if (ImGui::DragFloat("##X", &value, speed)) changed = true;;
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);
    ImGui::PopID();
    return changed;
}

template<typename T, typename UIFunction>
static void DrawComponent(const std::string& name, Actor actor, UIFunction uiFunction)
{
    const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
    if (actor.HasComponent<T>())
    {
        auto& component = actor.GetComponent<T>();
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

        //HEADING
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
        
        bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, name.c_str());
        //ImGui::PopStyleVar();
        
        ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
        if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
        {
            ImGui::OpenPopup("ComponentSettings");
        }
        bool removeComponent = false;
        if (ImGui::BeginPopup("ComponentSettings"))
        {
            if (ImGui::MenuItem("Remove Component")) removeComponent = true;

            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();

        //BODY
        if (open)
        {
            ImGui::Indent();
            ImGui::TreePop();
            uiFunction(component);
            
            Space();
            ImGui::Unindent();
        }
        if (removeComponent)
            actor.RemoveComponent<T>();
    }
}

int roundDown(int numToRound, int multiple)
{
    if (multiple == 0) return numToRound;
    return static_cast<int>(std::floor(static_cast<double>(numToRound)/static_cast<double>(multiple))*static_cast<double>(multiple));
}

void UIHierarchy::DrawComponents(Actor& actor)
{
    ImGui::Spacing();
    ImGui::Spacing();

    if (actor.HasComponent<TagComponent>())
    {
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
        auto& tag = actor.GetComponent<TagComponent>().Tag;
        ImGui::InputText("##Name", &tag);

        ImGui::SameLine(contentRegionAvailable.x - lineHeight * 1.0f);
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add", ImVec2{0, lineHeight}))
            ImGui::OpenPopup("AddComponent");

        ImGui::PopItemWidth();
        ImGui::PopStyleVar();

        if (ImGui::BeginPopup("AddComponent"))
        {
            if (!EditorLayer::selection.HasComponent<CameraComponent>()) if (ImGui::MenuItem("Camera")) { EditorLayer::selection.AddComponent<CameraComponent>(); ImGui::CloseCurrentPopup(); }
            if (!EditorLayer::selection.HasComponent<SpriteRendererComponent>()) if (ImGui::MenuItem("Sprite Renderer")) { EditorLayer::selection.AddComponent<SpriteRendererComponent>(); ImGui::CloseCurrentPopup(); }
            if (!EditorLayer::selection.HasComponent<TileMapComponent>()) if (ImGui::MenuItem("Tilemap")) { EditorLayer::selection.AddComponent<TileMapComponent>(); ImGui::CloseCurrentPopup(); }
            if (!EditorLayer::selection.HasComponent<BoxColliderComponent>()) if (ImGui::MenuItem("Box Collider")) { EditorLayer::selection.AddComponent<BoxColliderComponent>(); ImGui::CloseCurrentPopup(); }
            if (!EditorLayer::selection.HasComponent<NativeScriptComponent>()) if (ImGui::MenuItem("GameCloser")) { EditorLayer::selection.AddComponent<NativeScriptComponent>().Bind<CloseGameCollider>(); EditorLayer::selection.AddComponent<NativeScriptComponent>().Bind<CameraController>(); ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
        
        Space();
    }



    DrawComponent<TransformComponent>("Transform", actor, [](auto& component)
        {
            auto& transform = component.Transform;
            float* pos[2] = { &transform.Pos.x, &transform.Pos.y };
            float* scale[2] = { &transform.Scale.x, &transform.Scale.y };

            DrawVec2Control("Position", transform.Pos, 0.f, 1.f);
            DrawFloatControl("Rotation", transform.Rot, 0.f, 1.f);
            DrawVec2Control("Scale", transform.Scale, 1.f, 0.1f);
        });

    if (actor.GetComponent<RelationshipComponent>().Parent)
    {
        DrawComponent<RelationshipComponent>("Parent Attachment", actor, [](auto& component)
        {
            auto& transform = component.Offset;
            float* pos[2] = { &transform.Pos.x, &transform.Pos.y };
            float* scale[2] = { &transform.Scale.x, &transform.Scale.y };

            ImGui::Checkbox("Attach To Parent", &component.Attached);
            
            if (component.Attached)
            {
                ImGui::Text("Parent Offsets");
                DrawVec2Control("Rel Position", transform.Pos, 0.f, 1.f);
                DrawFloatControl("Rel Rotation", transform.Rot, 0.f, 1.f);
                DrawVec2Control("Rel Scale", transform.Scale, 1.f, 0.1f);
            }

        });
    }



        DrawComponent<SpriteRendererComponent>("Sprite Renderer", actor, [](auto& component)
            {
                std::string buf;
                ImGui::DragFloat("Z Order", &component.ZOrder);
                ImGui::Checkbox("Visible", &component.Visible);
                ImGui::InputText("Path", &component.Path);
                if (ImGui::Button("Load Texture"))
                {
                    component.Texture.loadFromFile(component.Path);
                    component.Sprite.setTexture(component.Texture, true);
                    component.Sprite.setOrigin(component.Texture.getSize().x / 2, component.Texture.getSize().y / 2);
                }

                ImGui::Spacing();
                ImGui::Spacing();
                float col[4] = { (float)component.Tint.r / 255.f, (float)component.Tint.g / 255.f, (float)component.Tint.b / 255.f, (float)component.Tint.a / 255.f };
                if (ImGui::ColorEdit4("Tint", col, ImGuiColorEditFlags_AlphaBar))
                {
                    sf::Color newcolor = sf::Color(col[0] * 255, col[1] * 255, col[2] * 255, col[3] * 255);
                    component.Tint = newcolor;
                }
            });

        DrawComponent<BoxColliderComponent>("Box Collider", actor, [](BoxColliderComponent& component)
        {
            float boxDims[2] = {component.Collider.width, component.Collider.height};
            sf::Vector2f Dims(component.Collider.width, component.Collider.height);
            ImGui::Checkbox("Active", &component.Active);
            ImGui::Checkbox("Preview", &component.Preview);
            if (!ImGui::Checkbox("Wrap to Sprite", &component.WrapToSprite))

            if (DrawVec2Control("Dimensions", Dims))
            {
                component.Collider.width = Dims.x;
                component.Collider.height = Dims.y;
            }
            DrawVec2Control("Offset", component.Offset);
        });

    DrawComponent<CameraComponent>("Camera", actor, [](auto& component)
        {
            ImGui::DragFloat("Zoom", &component.Zoom, 0.1f);
            ImGui::Checkbox("Primary", &component.Primary);
            ImGui::Checkbox("Inherit Rotation", &component.InheritRotation);
        });
    
    

    DrawComponent<TileMapComponent>("Tilemap", actor, [](TileMapComponent& component)
        {
            int* size[] = {&component.width, &component.height};
            int* tileSize[] = {&component.tileWidth, &component.tileHeight};
            ImGui::DragFloat("Z Order", &component.ZOrder);
            ImGui::Checkbox("Visible", &component.Visible);
            if(ImGui::DragInt2("Size", *size))
            {
                component.Load();
            }
            if(ImGui::DragInt2("Tile Size", *tileSize))
            {
                component.Load();
            }

            ImGui::InputText("SpriteSheet", &component.Path);

            
            
            double aspectRatioV = (double)component.Texture.getSize().y / (double)component.Texture.getSize().x;
            double aspectRatioH = (double)component.Texture.getSize().x / (double)component.Texture.getSize().y;
        
            ImVec2 image_pos = ImGui::GetCursorScreenPos();
            sf::Vector2f prevSize;
            prevSize.x = ImGui::GetContentRegionAvail().x;
            prevSize.y = aspectRatioV * ImGui::GetContentRegionAvail().x;

            float scaleFactor = prevSize.y / component.Texture.getSize().y;
        
            ImGui::Image(component.Texture, prevSize);
                if (ImGui::IsItemHovered())
                {
                    float MPosX = (ImGui::GetIO().MousePos.x - image_pos.x);
                    float MPosY = (ImGui::GetIO().MousePos.y - image_pos.y);
                    ImVec2 itemSize = ImGui::GetItemRectSize();
       
                    int TileX = (MPosX / (component.tileWidth * scaleFactor));
                    int TileY = (MPosY / (component.tileHeight * scaleFactor));
                    
                    
                    ImGui::GetWindowDrawList()->AddRect(ImVec2((image_pos.x + (TileX * component.tileWidth * scaleFactor)), (image_pos.y + (TileY * component.tileHeight * scaleFactor))),
                                                        ImVec2((image_pos.x + (TileX + 1) * component.tileWidth * scaleFactor), (image_pos.y + (TileY + 1) * component.tileHeight * scaleFactor)),
                                                        ImColor(255, 255,255, 255));
                    
                    if (ImGui::IsItemClicked())
                    {
                        std::cout << "X: " << TileX << " Y: " << TileY << std::endl;
                        int selection;
                        selection = TileX + (component.MaxTilesX * TileY);
                        component.SelectedTile = selection;
                    }
                }
        if (ImGui::Button("Fill Tilemap") && component.Visible)
        {
            for (int i = 0; i < component.Layout.size(); i++)
            {
                component.Layout[i] = component.SelectedTile;
                component.UpdateTexture(i);
            }
        }
        });
}