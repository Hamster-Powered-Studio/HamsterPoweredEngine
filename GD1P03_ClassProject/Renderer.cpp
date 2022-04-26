#include "Renderer.h"
#include "Components.h"
#include <iostream>

void Renderer::Init()
{
	View = new sf::RenderTexture();
	std::cout << "Initialised Renderer \n";
}

Renderer* Renderer::GetInstance()
{
	if (!Renderer::instance)
	{
		Renderer::instance = new Renderer();
		Renderer::Init();
	}
	return Renderer::instance;
}


void Renderer::BeginDraw()
{
	View->clear();
}

void Renderer::Draw(sf::Sprite& sprite, CameraComponent& camera)
{
	camera.Camera.setSize((sf::Vector2f)View->getSize() * camera.Zoom);
	View->setView(camera.Camera);
	View->draw(sprite);
}

void Renderer::EndDraw()
{
	View->display();
}

void Renderer::Resize(unsigned int width, unsigned int height)
{
	View->create(width, height);
	
	std::cout << ("Resized to: %i, %i", width, height) << std::endl;
}


Renderer* Renderer::instance = NULL;