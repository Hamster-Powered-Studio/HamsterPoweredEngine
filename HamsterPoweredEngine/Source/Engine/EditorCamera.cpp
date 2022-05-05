#include "EditorCamera.h"

#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include "Engine/Renderer.h"

EditorCamera::EditorCamera()
{
    setCenter(0, 0);
}

std::pair<float, float> EditorCamera::PanSpeed() const
{
    float x = std::min((float)m_ViewportSize.x / 1000.f, 2.4f);
    float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    float y = std::min((float)m_ViewportSize.y / 1000.f, 2.4f);
    float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

    return {xFactor, yFactor};
}

float EditorCamera::ZoomSpeed() const
{
    float newdistance = distance * 0.2f;
    newdistance = std::max(newdistance, 0.0f);
    float speed = newdistance * newdistance;
    speed = std::min(speed, 100.0f); // max speed = 100
    return speed;
}

void EditorCamera::MousePan(sf::Vector2f delta)
{
    setCenter(getCenter() + delta);
    auto [xSpeed, ySpeed] = PanSpeed();
    sf::Vector2f newCenter = getCenter();
    newCenter.x += delta.x * xSpeed * distance*2;
    newCenter.y += delta.y * ySpeed * distance*2;
    setCenter(newCenter);
}
void EditorCamera::MouseZoom(float delta)
{
    distance -= delta * ZoomSpeed();
    if (distance < 0.1f) distance = 1.f;
    
    setSize(m_ViewportSize.x * distance, m_ViewportSize.y * distance);
    
}

void EditorCamera::Resize(float x, float y)
{
    m_ViewportSize = {(unsigned)x, (unsigned)y};
    setSize(x * distance, y * distance);
}

void EditorCamera::OnUpdate(sf::Time deltaTime)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
    {
        const sf::Vector2i& mouse(sf::Mouse::getPosition());
        sf::Vector2f delta = (sf::Vector2f)(mouse - m_InitialMousePosition) * -1.f;
        m_InitialMousePosition = mouse;

        if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            MousePan(delta);
        }

        
    }
}

void EditorCamera::OnEvent(sf::Event& event)
{
    if (event.type == sf::Event::MouseWheelScrolled)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
            MouseZoom(event.mouseWheelScroll.delta);
    }
    if (event.type == sf::Event::Resized)
    {
        m_ViewportSize = Renderer::GetView()->getSize();
        setSize(m_ViewportSize.x, m_ViewportSize.y);
    }
}