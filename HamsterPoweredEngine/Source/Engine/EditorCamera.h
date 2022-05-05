#pragma once
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

class EditorCamera : public sf::View
{
public:
    EditorCamera();
    
    std::pair<float, float> PanSpeed() const;
    float ZoomSpeed() const;
    void MousePan(sf::Vector2f delta);
    void MouseZoom(float delta);
    void Resize(float x, float y);
    void OnUpdate(sf::Time deltaTime);
    void OnEvent(sf::Event& event);
    float distance = 1.f;
    sf::Vector2u m_ViewportSize;
private:
    sf::Vector2i m_InitialMousePosition;
    
};
