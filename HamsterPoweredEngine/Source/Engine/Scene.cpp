#include "Scene.h"

#include <imgui-SFML.h>
#include <imgui_internal.h>
#include <queue>

#include "Engine.h"
//#include "Global.h"
#include "Global.h"
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



void Scene::UpdateRelationships()
{
	{
		auto group = Registry.view<TransformComponent, RelationshipComponent>();
		for (auto actor : group)
		{
			auto [transform, relationship] = group.get<TransformComponent, RelationshipComponent>(actor);
			if (relationship.Parent && relationship.Attached)
			{
				TransformComponent parentTransformComp = relationship.Parent.GetComponent<TransformComponent>();
				transform.Transform = parentTransformComp.Transform + relationship.Offset;
			}
		}
	}
}




void Scene::OnUpdateEditor(float deltaTime, EditorCamera& camera)
{
	

	{
		Registry.view<LuaScriptComponent>().each([=](auto actor, auto& lsc)
		{
			//TODO: Move to Scene::OnScenePlay
			if (!lsc.Instance)
			{
				lsc.Instance = lsc.InstantiateScript();
				lsc.Instance->m_Actor = Actor{ actor, this };
				lsc.Instance->ReloadScripts();
			}
		});
	}

	UpdateRelationships();
	
		auto group = Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto actor : group)
		{
			const auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(actor);
			sprite.Sprite.setPosition(transform.Transform.Pos);
			sprite.Sprite.setRotation(transform.Transform.Rot);
			sprite.Sprite.setColor(sprite.Tint);

			if (sprite.Visible)
			{
				RenderItem elem{};
				elem.elem = &sprite.Sprite;
				elem.priority = sprite.ZOrder;
				Renderer::RenderQueue.push(elem);
			}
		}

	auto view = Registry.view<BoxColliderComponent, TransformComponent, MoveComponent>();
	for (auto actor : view)
	{
		auto [collider, transform, move] = view.get<BoxColliderComponent, TransformComponent, MoveComponent>(actor);
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
			collider.previewRect.setPosition(transform.Transform.Pos + collider.Offset);
			collider.previewRect.setOrigin(collider.Collider.width/2, collider.Collider.height/2);
			RenderItem prev;
			prev.elem = &collider.previewRect;
			prev.priority = 99999;
			Renderer::RenderQueue.emplace(prev);
		}
	}
		
		auto maps = Registry.view<TileMapComponent, TransformComponent>();
		for (auto actor : maps)
		{
			const auto& [tm, transform] = maps.get<TileMapComponent, TransformComponent>(actor);
			tm.map.setPosition(transform.Transform.Pos);

			if (tm.Visible)
			{
				RenderItem element{};
				element.elem = &tm.map;
				element.priority = tm.ZOrder;
				Renderer::RenderQueue.push(element);
			}
		}

		while (!Renderer::RenderQueue.empty())
		{
			Renderer::Draw(*Renderer::RenderQueue.top().elem, &camera);
			Renderer::RenderQueue.pop();
		}
}

void Scene::OnUpdateRuntime(float deltaTime)
{
	std::priority_queue<RenderItem> RenderQueue;

	//Update Scripts
	{
		Registry.view<NativeScriptComponent>().each([=](auto actor, auto& nsc)
		{
			//TODO: Move to Scene::OnScenePlay
			if (!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Actor = Actor{ actor, this };
				nsc.Instance->OnCreate();
			}
			nsc.Instance->OnUpdate(deltaTime);
		});
	}

	{
		Registry.view<LuaScriptComponent>().each([=](auto actor, auto& lsc)
		{
			//TODO: Move to Scene::OnScenePlay
			if (!lsc.Instance)
			{
				lsc.Instance = lsc.InstantiateScript();
				lsc.Instance->m_Actor = Actor{ actor, this };
				lsc.Instance->OnCreate();
				lsc.Instance->ReloadScripts();
			}
			if (lsc.Scripts.size() > 0)
				lsc.Instance->OnUpdate(deltaTime);
			//std::cout << 1.f / global::deltaTime << std::endl;
		});
	}
	
	UpdateRelationships();
	

	{
		auto view = Registry.view<BoxColliderComponent, TransformComponent, MoveComponent>();
		for (auto actor : view)
		{
			
			auto [collider, transform, move] = view.get<BoxColliderComponent, TransformComponent, MoveComponent>(actor);
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
				collider.previewRect.setPosition(transform.Transform.Pos + collider.Offset);
				collider.previewRect.setOrigin(collider.Collider.width/2, collider.Collider.height/2);
				RenderItem prev;
				prev.elem = &collider.previewRect;
				prev.priority = 99999;
				Renderer::RenderQueue.emplace(prev);
			}

			//if (move.Move.x == 0 && move.Move.y == 0) continue;

			sf::FloatRect screen = Renderer::GetView()->getView().getViewport();
			if (!screen.intersects(collider.Collider)) continue;
			
			
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
							move.Move.x = 0;
							collider.IsColliding = true;
							collider.Other = Actor{otheractor, this};
							
						}
						if (futureColliderUD.intersects(othercollider.Collider))
						{
							collider.IsColliding = true;
							move.Move.y = 0;
							collider.Other = Actor{otheractor, this};
						}
						if (collider.IsColliding == false) collider.Other = {}; 
					}
				}
			}
		}
	}

	{
		auto view = Registry.view<TransformComponent, MoveComponent>();
		for (auto actor : view)
		{
			auto [transform, move] = view.get<TransformComponent, MoveComponent>(actor);
			{
				transform.Transform.Pos += move.Move;
				move.Move = {0, 0};
			}
		}
	}
	
	
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
				bool resize = false;
				if (mainCamera != &camera) resize = true;
				mainCamera = &camera;
				
				//if (resize) global::Game->ResizeGameView();
			}
		}
	}
	
	
	if (mainCamera)
	{

		if (global::Game->editor)	
			if (mainCamera->Camera.getSize() != sf::Vector2f(Renderer::GetView()->getSize().x, Renderer::GetView()->getSize().y))
			{
				mainCamera->Camera.setSize(Renderer::GetView()->getSize().x, Renderer::GetView()->getSize().y);
			}
		
		auto group = Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto actor : group)
		{
			const auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(actor);
			sprite.Sprite.setPosition(transform.Transform.Pos);
			sprite.Sprite.setRotation(transform.Transform.Rot);
			sprite.Sprite.setColor(sprite.Tint);

			if (sprite.Visible)
			{
				RenderItem elem{};
				elem.elem = &sprite.Sprite;
				elem.priority = sprite.ZOrder;
				Renderer::RenderQueue.push(elem);
			}
			
		}
		
		auto maps = Registry.view<TileMapComponent, TransformComponent>();
		for (auto actor : maps)
		{
			const auto& [tm, transform] = maps.get<TileMapComponent, TransformComponent>(actor);
			tm.map.setPosition(transform.Transform.Pos);
			sf::Vector2f boundsSize((float)tm.width * (float)tm.tileWidth, (float)tm.height * (float)tm.tileHeight);
			sf::Vector2f boundsPos(transform.Transform.Pos.x - (boundsSize.x / 2), transform.Transform.Pos.y - (boundsSize.y / 2));
			sf::FloatRect bounds(boundsPos, boundsSize);
			tm.Bounds = bounds;
			
			if (tm.Visible)
			{
				RenderItem element{};
				element.elem = &tm.map;
				element.priority = tm.ZOrder;
				Renderer::RenderQueue.push(element);
			}
		}

	while (!Renderer::RenderQueue.empty())
		{
			Renderer::Draw(*Renderer::RenderQueue.top().elem, &mainCamera->Camera);
			Renderer::RenderQueue.pop();
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

Actor Scene::DuplicateActor(Actor actor)
{
	return Actor();
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

template<>
void Scene::OnComponentAdded<NativeScriptComponent>(Actor actor, NativeScriptComponent& component)
{	
}

template<>
void Scene::OnComponentAdded<LuaScriptComponent>(Actor actor, LuaScriptComponent& component)
{	
}