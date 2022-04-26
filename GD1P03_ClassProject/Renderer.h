#pragma once

#include "SFML/Graphics.hpp"
class CameraComponent;

class Renderer
{
private:
	static Renderer* instance;
	static inline sf::RenderTexture* View;
private:
	Renderer() {}
public:

	static Renderer* GetInstance();
	static void Init();
	

	static void BeginDraw();
	static void Draw(sf::Sprite& sprite, CameraComponent& camera);
	static void EndDraw();
	static void Resize(unsigned int width, unsigned int height);

	
	static sf::RenderTexture* GetView() { return View; }

};

