#pragma once
#include "Engine/ScriptableActor.h"
#include "Components/Components.h"
#include <iostream>

class CameraController : public ScriptableActor
{
public:
    void OnCreate()
    {
        
    }

    void OnDestroy()
    {
            
    }

    void OnUpdate(float deltaTime)
    {
        auto& transform = GetComponent<TransformComponent>().Transform;
        float speed = 500.0f;
            
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            transform.Pos.y -= speed * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            transform.Pos.x -= speed * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            transform.Pos.y += speed * deltaTime;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            transform.Pos.x += speed * deltaTime;
        }
    }
};