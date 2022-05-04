#include "Scene.h"

#include <imgui-SFML.h>
#include <imgui_internal.h>
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
	std::priority_queue<PQElement> RenderQueue;
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
		auto group = Registry.view<MoveComponent, InputComponent>();
		for (auto actor : group)
		{
			const auto& [move, input] = group.get<MoveComponent, InputComponent>(actor);
			bool colliding = false;
			if (Registry.any_of<BoxColliderComponent>(actor))
			{
				colliding = Registry.get<BoxColliderComponent>(actor).IsColliding;
			}
			
			if (input.Active && !colliding)
			{
				
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				{
					move.Move += {0, -1};
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				{
					move.Move += {-1, 0};
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				{
					move.Move += {0, 1};
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				{
					move.Move += {1, 0};
				}
			}
		}
	}

	{
		auto view = Registry.view<BoxColliderComponent, TransformComponent, MoveComponent>();
		for (auto actor : view)
		{
			const auto& [collider, transform, move] = view.get<BoxColliderComponent, TransformComponent, MoveComponent>(actor);
			if (collider.WrapToSprite)
			{
				if (Registry.any_of<SpriteRendererComponent>(actor))
				{
					auto& spr = Registry.get<SpriteRendererComponent>(actor);
					collider.Collider.width = spr.Sprite.getTexture()->getSize().x;
					collider.Collider.height = spr.Sprite.getTexture()->getSize().y;
				}
			}
			
			collider.Collider.left = transform.Transform.Pos.x - (collider.Collider.width/2);
			collider.Collider.top = transform.Transform.Pos.y - (collider.Collider.height/2);

			if (collider.Preview)
			{
				collider.previewRect.setSize({collider.Collider.width, collider.Collider.height});
				collider.previewRect.setPosition(transform.Transform.Pos);
				collider.previewRect.setOrigin(collider.Collider.width/2, collider.Collider.height/2);
				PQElement prev;
				prev.elem = &collider.previewRect;
				prev.priority = 99999;
				RenderQueue.emplace(prev);
			}

			sf::FloatRect futureColliderLR(collider.Collider.left + move.Move.x, collider.Collider.top, collider.Collider.width, collider.Collider.height);
			sf::FloatRect futureColliderUD(collider.Collider.left, collider.Collider.top + move.Move.y, collider.Collider.width, collider.Collider.height);
			
			if (collider.Active)
			{
				for (auto otheractor : view)
				{
					if(otheractor == actor) continue;
					const auto& othercollider = view.get<BoxColliderComponent>(otheractor);
					if (othercollider.Active)
					{
						if (futureColliderLR.intersects(othercollider.Collider))
						{
							std::cout << "COLLISION";
							move.Move.x = 0;
							//collider.IsColliding = true;
						}
						if (futureColliderUD.intersects(othercollider.Collider))
						{
							move.Move.y = 0;
						}
					}
				}
			}
		}
	}

	{
		auto view = Registry.view<TransformComponent, MoveComponent>();
		for (auto actor : view)
		{
			const auto& [transform, move] = view.get<TransformComponent, MoveComponent>(actor);
			{
				transform.Transform.Pos += move.Move;
				move.Move = {0, 0};
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
	actor.AddComponent<MoveComponent>();
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

template<>
void Scene::OnComponentAdded<BoxColliderComponent>(Actor actor, BoxColliderComponent& component)
{	
}

template<>
void Scene::OnComponentAdded<MoveComponent>(Actor actor, MoveComponent& component)
{	
}
