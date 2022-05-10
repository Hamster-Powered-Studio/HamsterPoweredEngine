#pragma once
#include <imgui.h>

#include "Components/Components.h"
#define ITEM(x) case sf::Keyboard::isKeyPressed ## x : ret = #x; break;


namespace LuaBindings
{
    static bool IsKeyDown(std::string key)
    {
        if(key == "W") return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
        if(key == "A") return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
        if(key == "S") return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
        if(key == "D") return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
        if(key == "E") return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E);
        if(key == "F") return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F);
        if(key == "Q") return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q);
        if(key == "Space") return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
        if(key == "Esc") return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
        return false;
    }

    static bool IsMouseDown(int button)
    {
        if(button == 0) return sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        if(button == 1) return sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
        if(button == 2) return sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle);
       
        return false;
    }

    inline BoxColliderComponent CopyBox(BoxColliderComponent& box)
    {
        return BoxColliderComponent(box);
    }
    
    static void RegisterBindings(sol::state& lua, Actor actor = {})
    {
        lua.open_libraries(sol::lib::base);
        lua.open_libraries(sol::lib::math);
        
        lua["Scene"] = actor.GetScene();
        lua["Self"] = actor;
        //std::vector<BoxColliderComponent>
        lua.new_usertype<Scene>("ScenePiece",
            "GetByUUID", &Scene::GetByUUID,
            "GetAllColliders", &Scene::GetAllComponents<BoxColliderComponent>,
            "GetAllTilemaps", &Scene::GetAllComponents<TileMapComponent>);
        
        lua.new_usertype<Actor>("Actor",
            "GetTransform", &Actor::GetComponent<TransformComponent>,
            "GetCollider", &Actor::GetComponent<BoxColliderComponent>,
            "GetTag", &Actor::GetComponent<TagComponent>,
            "GetMoveComponent", &Actor::GetComponent<MoveComponent>,
            "GetRelationship", &Actor::GetComponent<RelationshipComponent>);
        lua.new_usertype<TransformComponent>("TransformComponent",
            "Transform", &TransformComponent::Transform);
        lua.new_usertype<Transform2D>("Transform2D",
            "Pos", &Transform2D::Pos,
            "Rot", &Transform2D::Rot,
            "Scale", &Transform2D::Scale);
        lua.new_usertype<sf::Vector2f>("Vector2f",
            "x", &sf::Vector2f::x,
            "y", &sf::Vector2f::y);
        lua.new_usertype<BoxColliderComponent>("BoxColliderComponent",
            "IsColliding", &BoxColliderComponent::IsColliding,
            "Bounds", &BoxColliderComponent::Collider,
            "OtherActor", &BoxColliderComponent::Other,
            "Offset", &BoxColliderComponent::Offset,
            "CopyBox", &LuaBindings::CopyBox,
            "Type", &BoxColliderComponent::Type,
            "Active", &BoxColliderComponent::Active);
        lua.new_usertype<sf::FloatRect>("FloatRect",
            "Height", &sf::FloatRect::height,
            "Width", &sf::FloatRect::width,
            "Left", &sf::FloatRect::left,
            "Top", &sf::FloatRect::top);
        lua.new_usertype<RelationshipComponent>("RelationshipComponent",
            "Parent", &RelationshipComponent::Parent,
            "Children", &RelationshipComponent::children,
            "Offset", &RelationshipComponent::Offset,
            "IsAttached", &RelationshipComponent::Attached);
        lua.new_usertype<TileMapComponent>("TileMapComponent",
            "Height", &TileMapComponent::height,
            "Width", &TileMapComponent::width,
            "TileHeight", &TileMapComponent::tileHeight,
            "TileWidth", &TileMapComponent::tileWidth,
            "Tiles", &TileMapComponent::Layout,
            "Bounds", &TileMapComponent::Bounds,
            "Colliders", &TileMapComponent::Colliders);
        lua.new_usertype<TagComponent>("TagComponent",
            "Tag", &TagComponent::Tag);
        lua.new_usertype<MoveComponent>("MoveComponent",
            "Vector", &MoveComponent::Move);
        lua["IsMouseDown"] = &LuaBindings::IsMouseDown;
        lua["IsKeyDown"] = &LuaBindings::IsKeyDown;
        
    }
    
}
