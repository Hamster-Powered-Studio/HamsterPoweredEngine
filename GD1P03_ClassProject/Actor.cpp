#include "Actor.h"
#include "Engine.h"


Actor::Actor(entt::entity handle, Scene* scene) : m_ActorHandle(handle), m_Scene(scene)
{
}


/* OLD
Actor::Actor()
{
    ActorNum++;
    ActorMax++;
    InstanceNum = ActorMax;
}

Actor::Actor(sf::Texture& texture)
{
    LoadTexture(texture);
    ActorNum++;
    ActorMax++;
    InstanceNum = ActorMax;
}

Actor::Actor(std::string texturePath)
{
    LoadTexture(texturePath);
    ActorNum++;
    ActorMax++;
    InstanceNum = ActorMax;
}

Actor::~Actor()
{
    ActorNum--;
}



void Actor::Update()
{
    if (parent)
    {
        if (snapToParent)
            setPosition(parent->getPosition() + offsetFromParent);
    }
}

std::string Actor::GetClassName()
{
    return displayName;
}

void Actor::Attach(Actor* NewParent, bool SnapToParent)
{
    if(!NewParent) return;
    parent = NewParent;
    if (SnapToParent)
    {
        snapToParent = true;
        setPosition(NewParent->getPosition());
    }
    offsetFromParent = parent->getPosition() - getPosition();
    NewParent->children.push_back(this);
}

void Actor::Detach()
{
    parent = nullptr;
    offsetFromParent = {0, 0};
}

sf::Vector2f Actor::GetSize()
{
    return {width, height};
}

void Actor::SetRelativePosition(Actor& relativeTo, sf::Vector2f pos)
{
    sf::Vector2f newPos = relativeTo.getPosition() + pos;
    setPosition(newPos);

    if(parent)
    {
        offsetFromParent = parent->getPosition() - getPosition();
    }
}

sf::Vector2f Actor::GetRelativePosition(Actor& relativeTo)
{
    sf::Vector2f relativePos;
    relativePos = getPosition() - relativeTo.getPosition();
    return relativePos;
}

void Actor::DisplayExposedVars()
{
    ImGui::Text("Actor Params");
    ImGuiInputTextCallback cb;
    
    float pos[2] = { getPosition().x, getPosition().y };
    if(ImGui::DragFloat2("World Position", pos, 1)) setPosition(pos[0], pos[1]);
    
    float scale[2] = { getScale().x, getScale().y};
    ImGui::Checkbox("Lock", &lockScale);
    ImGui::SameLine();
    
    
    if (ImGui::DragFloat2("World Scale", scale, 0.1f, 0.0f, 0.0f, "% .3f"))
    {
        float w = scale[0];
        float h = scale[1];

        if (w != 0 && h != 0)
        {
            hRatio = getScale().y / getScale().x;
            wRatio = getScale().x / getScale().y;
        }
        
        
        if (lockScale == true)
        {
            
                if (w != getScale().x)
                {
                    h = w * hRatio;
                    setScale(w, h);
                    
                }
                else if (h != getScale().y)
                {
                    w = h * wRatio;
                    setScale(w, h);
                    //setScale(w, h);
                }
                
                //h = w * hRatio;

             
        }
        else
        {
            setScale(w, h);
        }

        
    }

    if (ImGui::InputText("Texture", &texturePath, ImGuiInputTextFlags_EnterReturnsTrue)) LoadTexture(texturePath);;

    ImGui::Checkbox("Visible", &visible);

    if (parent)
    {
        ImGui::Checkbox("Follow Parent", &snapToParent);
        float* relpos[2] = { &offsetFromParent.x, &offsetFromParent.y };
        ImGui::DragFloat2("Offset from Parent", *relpos, 1.f, -10000.f, 10000.f);
        if (!snapToParent) {
            if (ImGui::Button("Lock Current Offset")) {
                offsetFromParent = GetRelativePosition(*parent);
                snapToParent = true;
            }
               
        }
    }

}

void Actor::AddWorldOffset(sf::Vector2f offset)
{
    sf::Vector2f newPos = getPosition() + offset;
    setPosition(newPos);
}



bool Actor::LoadTexture(std::string path)
{

    if (Texture.loadFromFile(path));
    {
        width = (float)Texture.getSize().x;
        height = (float)Texture.getSize().y;
        setTexture(this->Texture, true);
        setOrigin(width / 2, height / 2);
        return true;
    }

    return false;

}

bool Actor::LoadTexture(sf::Texture& texture)
{
    Texture = texture;
    width = (float)Texture.getSize().x;
    height = (float)Texture.getSize().y;
    setTexture(this->Texture, true);
    setOrigin(width/2, height/2);
    return true;
    
}

*/

