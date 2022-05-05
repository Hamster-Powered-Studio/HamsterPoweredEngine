#pragma once

#include "SFML/Graphics.hpp"
struct CameraComponent;
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
	static void Draw(sf::Drawable& sprite, sf::View* camera);
	static void EndDraw();
	static void Resize(unsigned int width, unsigned int height);
	
	static sf::RenderTexture* GetView() { return View; }
};

