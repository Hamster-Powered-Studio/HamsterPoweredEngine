#include "Scene.h"

#include <queue>

#include "Engine.h"
#include "Components/Components.h"
#include "Engine/HPUUID.h"

Scene::Scene()
{

	

}

Scene::~Scene()
{
}

Actor Scene::GetByUUID(HPUUID uuid)
{
	{
		auto view = Registry.view<IDComponent>();
		for(auto& actor : view)
		{
			const auto& id = view.get<IDComponent>(actor);
			if (id.ID == uuid) return Actor(actor, this);
		}
		return {};
	}
}

struct PQElement
{
	sf::Drawable* elem;
	float priority;

	//Overload the < operator.
	bool operator< (const PQElement &other) const
	{
		return priority > other.priority;
	}
	//Overload the > operator.
	bool operator> (const PQElement &other) const
	{
		return priority < other.priority;
	}
};

void Scene::Update(sf::Time deltaTime)
{

	{
		auto group = Registry.view<TransformComponent, RelationshipComponent>();
		for (auto actor : group)
		{
			const auto& [transform, relationship] = group.get<TransformComponent, RelationshipComponent>(actor);
			if (relationship.Parent && relationship.Attached)
			{
				TransformComponent parentTransformComp = relationship.Parent.GetComponent<TransformComponent>();
				transform.Transform = parentTransformComp.Transform + relationship.Offset;
			}
		}
	}
	
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
		std::priority_queue<PQElement> RenderQueue;
		auto group = Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto actor : group)
		{
			const auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(actor);
			sprite.Sprite.setPosition(transform.Transform.Pos);
			sprite.Sprite.setRotation(transform.Transform.Rot);
			sprite.Sprite.setColor(sprite.Tint);

			if (sprite.Visible)
			{
				PQElement elem{};
				elem.elem = &sprite.Sprite;
				elem.priority = sprite.ZOrder;
				RenderQueue.push(elem);
			}
			
		}
		
		auto maps = Registry.view<TileMapComponent, TransformComponent>();
		for (auto actor : maps)
		{
			const auto& [tm, transform] = maps.get<TileMapComponent, TransformComponent>(actor);
			tm.map.setPosition(transform.Transform.Pos);

			if (tm.Visible)
			{
				PQElement element{};
				element.elem = &tm.map;
				element.priority = tm.ZOrder;
				RenderQueue.push(element);
			}
		}

	while (!RenderQueue.empty())
		{
			Renderer::Draw(*RenderQueue.top().elem, mainCamera);
			RenderQueue.pop();
		}
	}
	
	
}

Actor Scene::CreateActor(const std::string& name)
{
	return CreateActorWithUUID(HPUUID(), name);
}

Actor Scene::CreateActorWithUUID(HPUUID uuid, const std::string& name)
{
	std::cout << "ActorSpawned!";
	Actor actor = { Registry.create(), this };
	actor.AddComponent<IDComponent>(uuid);
	actor.AddComponent<TransformComponent>();
	actor.AddComponent<RelationshipComponent>();
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
}

template<>
void Scene::OnComponentAdded<IDComponent>(Actor actor, IDComponent& component)
{
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

template<>
void Scene::OnComponentAdded<RelationshipComponent>(Actor actor, RelationshipComponent& component)
{	
}
