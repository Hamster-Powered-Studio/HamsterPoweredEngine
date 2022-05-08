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
        return false;
    }

    static bool IsMouseDown(int button)
    {
        if(button == 0) return sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        if(button == 1) return sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
        if(button == 2) return sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle);
       
        return false;
    }
    
    static void RegisterBindings(sol::state& lua)
    {
        lua.new_usertype<TransformComponent>("TransformComponent",
            "GetTransform", &TransformComponent::Transform);
        lua.new_usertype<Transform2D>("Transform2D",
            "Pos", &Transform2D::Pos,
            "Rot", &Transform2D::Rot,
            "Scale", &Transform2D::Scale);
        lua.new_usertype<sf::Vector2f>("Vector2f",
            "x", &sf::Vector2f::x,
            "y", &sf::Vector2f::y);
        lua.new_usertype<BoxColliderComponent>("BoxColliderComponent",
            "IsColliding", &BoxColliderComponent::IsColliding,
            "Size", &BoxColliderComponent::Collider,
            "OtherActor", &BoxColliderComponent::Other,
            "Offset", &BoxColliderComponent::Offset);
        lua.new_usertype<sf::FloatRect>("FloatRect",
            "Height", &sf::FloatRect::height,
            "Width", &sf::FloatRect::width,
            "Left", &sf::FloatRect::left,
            "Top", &sf::FloatRect::top);
        lua["IsMouseDown"] = &LuaBindings::IsMouseDown;
        lua["IsKeyDown"] = &LuaBindings::IsKeyDown;
        
    }
    
}
