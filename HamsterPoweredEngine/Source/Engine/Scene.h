#pragma once

#include "entt.hpp"
#include <SFML/System/Clock.hpp>

#include "HPUUID.h"
//#include "Components/Components.h"
#include "Engine/EditorCamera.h"
#include "Engine/GameState.h"

struct CameraComponent;
struct Actor;

class Scene
{
public:
	Scene();
	~Scene();
	Actor GetByUUID(HPUUID uuid);

	void OnUpdateRuntime(float deltaTime);
	void UpdateRelationships();
	void OnUpdateEditor(float deltaTime, EditorCamera& camera);
	Actor SpawnProjectile(float xpos, float ypos, float xspeed, float yspeed);

	Scene* get()
	{
		return this;
	}

	Actor CreateActor(const std::string& name = std::string());
	Actor CreateActorWithUUID(HPUUID uuid, const std::string& name);
	Actor DuplicateActor(Actor actor);
	Actor FindByName(std::string name);
	GameState& GetGameState();
	void OpenLevel(std::string path);
	void DestroyActor(Actor actor);
	void AddToDestroyList(Actor actor);
	entt::registry& Reg() { return Registry; }
	CameraComponent* mainCamera = nullptr;
	
	template<typename T>
	std::vector<T*> GetAllComponents()
	{
		std::vector<T*> components;
		Registry.view<T>().each([&](auto& component){components.push_back(&component);});
		return components;
	}
	
private:
	template<typename T>
	void OnComponentAdded(Actor actor, T& component);
	std::vector<entt::entity> destroyList;
private:
	entt::registry Registry;
	
	friend struct Actor;
	friend class SceneSerializer;
	friend class UIHierarchy;
};

