#pragma once

#include "SFML/Graphics.hpp"
#include <queue>
struct CameraComponent;

struct RenderItem
{
	sf::Drawable* elem;
	float priority;

	//Overload the < operator.
	bool operator< (const RenderItem &other) const
	{
		return priority > other.priority;
	}
	//Overload the > operator.
	bool operator> (const RenderItem &other) const
	{
		return priority < other.priority;
	}
};

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

	

	static inline std::priority_queue<RenderItem> RenderQueue;
	
	static sf::RenderTexture* GetView() { return View; }
};

