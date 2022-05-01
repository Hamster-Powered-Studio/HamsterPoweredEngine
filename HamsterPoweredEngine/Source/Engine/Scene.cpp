#include "Scene.h"
#include "Engine.h"
#include "Components/Components.h"

Scene::Scene()
{

	

}

Scene::~Scene()
{
}

void Scene::Update(sf::Time deltaTime)
{
	{
		auto group = Registry.view<TransformComponent, InputComponent>();
		for (auto actor : group)
		{
			const auto& [transform, input] = group.get<TransformComponent, InputComponent>(actor);

			if (input.Active)
			{
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				{
					transform.Transform.Pos += {0, -1};
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				{
					transform.Transform.Pos += {-1, 0};
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				{
					transform.Transform.Pos += {0, 1};
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				{
					transform.Transform.Pos += {1, 0};
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
				{
					transform.Transform.Rot -= 0.1f;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
				{
					transform.Transform.Rot += 0.1f;
				}
			}
		}
	}

	CameraComponent* mainCamera = nullptr;
	{
		auto group = Registry.view<TransformComponent, CameraComponent>();
		for (auto actor : group)
		{
			const auto& [transform, camera] = group.get<TransformComponent, CameraComponent>(actor);

			camera.Camera.setCenter(transform.Transform.Pos);

			if (camera.InheritRotation)
				camera.Camera.setRotation(transform.Transform.Rot);
			else camera.Camera.setRotation(0);

			if (camera.Primary)
			{
				mainCamera = &camera;
			}
		}
	}

	if (mainCamera)
	{
		
		auto group = Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto actor : group)
		{
			const auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(actor);
			sprite.Sprite.setPosition(transform.Transform.Pos);
			sprite.Sprite.setRotation(transform.Transform.Rot);
			sprite.Sprite.setColor(sprite.Tint);

			if (sprite.Visible)
			Renderer::Draw(sprite.Sprite, mainCamera);
		}
		
		auto maps = Registry.view<TileMapComponent, TransformComponent>();
		for (auto actor : maps)
		{
			const auto& [tm, transform] = maps.get<TileMapComponent, TransformComponent>(actor);
			tm.map.setPosition(transform.Transform.Pos);
			Renderer::Draw(tm.map, mainCamera);
		}
	}
	
	
}

Actor Scene::CreateActor(const std::string& name)
{
	std::cout << "ActorSpawned!";
	Actor actor = { Registry.create(), this };
	actor.AddComponent<TransformComponent>();
	auto& tag = actor.AddComponent<TagComponent>();
	tag.Tag = name.empty() ? "Actor" : name;
	return actor;
}

void Scene::DestroyActor(Actor actor)
{
	if (Registry.valid(actor))
		Registry.destroy(actor);
}

template<typename T>
void Scene::OnComponentAdded(Actor actor, T& component)
{
	//std::cout << "WRONG" << std::endl;
	//static_assert(false);
	assert(false, "Must specify component type");
}

template<>
void Scene::OnComponentAdded<TransformComponent>(Actor actor, TransformComponent& component)
{
}

template<>
void Scene::OnComponentAdded<TagComponent>(Actor actor, TagComponent& component)
{
}

template<>
void Scene::OnComponentAdded<CameraComponent>(Actor actor, CameraComponent& component)
{
}

template<>
void Scene::OnComponentAdded<InputComponent>(Actor actor, InputComponent& component)
{
}

template<>
void Scene::OnComponentAdded<SpriteRendererComponent>(Actor actor, SpriteRendererComponent& component)
{
}

template<>
void Scene::OnComponentAdded<TileMapComponent>(Actor actor, TileMapComponent& component)
{	
}
