#pragma once

#include "entt.hpp"
#include <SFML/System/Clock.hpp>

#include "HPUUID.h"
//#include "Components/Components.h"
#include "Engine/EditorCamera.h"

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

	Scene* get()
	{
		return this;
	}

	Actor CreateActor(const std::string& name = std::string());
	Actor CreateActorWithUUID(HPUUID uuid, const std::string& name);
	void DestroyActor(Actor actor);
	entt::registry& Reg() { return Registry; }
	CameraComponent* mainCamera = nullptr;
private:
	template<typename T>
	void OnComponentAdded(Actor actor, T& component);

private:
	entt::registry Registry;
	
	friend struct Actor;
	friend class SceneSerializer;
	friend class UIHierarchy;
};

