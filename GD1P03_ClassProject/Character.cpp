

#include "Character.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Window.hpp>
#include <cmath>
#include "Global.h"
#include "imgui.h"
#include "Level.h"

/*
Character::Character()
{
    PlayerCam = new Camera();
    PlayerCam->Attach(this, true);
    PlayerCam->displayName = "Main Player Camera";
    Possess();
   // Actor* tempCam = global::World->SpawnActor<Camera>(});
    
    displayName = "Character";
}

Character::~Character()
{

}

void Character::Update()
{
    Actor::Update();
    

    if(IsPossessed && global::Game->window->hasFocus())
    {
        sf::Vector2f moveDir = {0, 0};
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)) moveDir += {0, -1};
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) moveDir += {-1, 0};
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) moveDir += {0, 1};
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) moveDir += {1, 0};

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::X)) PlayerCam->Detach();
        moveDir *= moveSpeed;
        moveDir *= global::deltaTime.asSeconds();

        AddWorldOffset(moveDir);
    }
    PlayerCam->Update();

    if (EDITOR)
    {
    
    }
}


void Character::Possess()
{
    IsPossessed = true;
    PlayerCam->SetActiveCamera();
}

void Character::Unpossess()
{
    IsPossessed = false;
}

void Character::DisplayExposedVars()
{
    Actor::DisplayExposedVars();
    ImGui::DragFloat("Speed", &moveSpeed, 1, 0, 1000);
}
*/