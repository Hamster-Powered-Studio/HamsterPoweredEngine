#pragma once

#include "entt.hpp"
#include <SFML/System/Clock.hpp>

#include "HPUUID.h"
#include "Engine/EditorCamera.h"

struct Actor;

class Scene
{
public:
	Scene();
	~Scene();
	Actor GetByUUID(HPUUID uuid);

	void OnUpdateRuntime(sf::Time deltaTime);
	void OnUpdateEditor(sf::Time deltaTime, EditorCamera& camera);

	Scene* get()
	{
		return this;
	}

	Actor CreateActor(const std::string& name = std::string());
	Actor CreateActorWithUUID(HPUUID uuid, const std::string& name);
	void DestroyActor(Actor actor);
	entt::registry& Reg() { return Registry; }

private:
	template<typename T>
	void OnComponentAdded(Actor actor, T& component);

private:
	entt::registry Registry;
	
	friend struct Actor;
	friend class SceneSerializer;
	friend class UIHierarchy;
};

