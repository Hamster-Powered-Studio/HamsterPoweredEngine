#pragma once
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/View.hpp>

#include "Actor.h"

/*
class Camera : public Actor
{
public:
    Camera();
    ~Camera();
    
    sf::Vector2f playerCameraPosition;
    sf::View currentView;
    
    Actor* parent;
    float zoom = 1.f;

    void Update() override;
    void SetActiveCamera();

    void DisplayExposedVars() override;
    
};
*/