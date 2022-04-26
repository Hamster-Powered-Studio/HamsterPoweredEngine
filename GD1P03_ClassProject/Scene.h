#pragma once

#include "entt.hpp"
#include <SFML/System/Clock.hpp>

class Actor;

class Scene
{
public:
	Scene();
	~Scene();

	void Update(sf::Time deltaTime);

	Scene* get()
	{
		return this;
	}

	Actor CreateActor(const std::string& name = std::string());
	void DestroyActor(Actor actor);
	entt::registry& Reg() { return Registry; }

private:
	template<typename T>
	void OnComponentAdded(Actor actor, T& component);

private:
	entt::registry Registry;
	
	friend class Actor;
	friend class SceneSerializer;
	friend class UIHierarchy;
};

