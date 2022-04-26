#include "Camera.h"
#include "Global.h"

/*
Camera::Camera() : parent(nullptr)
{
    sf::Vector2u size = global::Game->PrevRender.getSize();
    currentView = sf::View();
    currentView.setCenter(getPosition());
    currentView.setSize((float)size.x, (float)size.y);

}

Camera::~Camera()
{
    
}


void Camera::Update()
{
    Actor::Update();
    //std::cout << getPosition().x << " " << getPosition().y << std::endl;
    sf::Vector2u size = global::Game->PrevRender.getSize();
    currentView.setCenter(getPosition());
    currentView.setSize((float)size.x, (float)size.y);
    zoom = std::clamp(zoom, 0.1f, 10.f);
    currentView.zoom(zoom);

    if(global::event)
    {
        if(global::event->type == sf::Event::Resized)
        {
            sf::Vector2u size = global::Game->window->getSize();
            currentView.setCenter(getPosition());
            currentView.setSize((float)size.x, (float)size.y);
        }
    }
    
    SetActiveCamera();
}

void Camera::SetActiveCamera()
{
    global::Game->window->setView(currentView);
    global::Game->PrevRender.setView(currentView);
}

void Camera::DisplayExposedVars()
{
    Actor::DisplayExposedVars();

    ImGui::Spacing();
    ImGui::Text("Unique Params", "");
    if(ImGui::DragFloat("Zoom", &zoom, 0.1f, 10.f, 0.f)) ;


}



*/