#include "Scene.h"

#include <imgui-SFML.h>
#include <imgui_internal.h>
#include <queue>

#include "Engine.h"
//#include "Global.h"
#include "Global.h"
#include "ProjectileLuaActor.h"
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
			sprite.Sprite.setScale(transform.Transform.Scale);
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
				collider.Collider.width = spr.Sprite.getTexture()->getSize().x * transform.Transform.Scale.x;
				collider.Collider.height = spr.Sprite.getTexture()->getSize().y * transform.Transform.Scale.y;
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
			tm.map.setScale(transform.Transform.Scale);
			tm.map.setRotation(transform.Transform.Rot);
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

Actor Scene::SpawnProjectile(float xpos, float ypos, float xspeed, float yspeed)
{
	Actor proj = CreateActor("Projectile");
	auto& spr = proj.AddComponent<SpriteRendererComponent>();
	auto& transform = proj.GetComponent<TransformComponent>();
	auto& script = proj.AddComponent<LuaScriptComponent>();
	auto& collider = proj.AddComponent<BoxColliderComponent>();
	

	transform.Transform.Pos = sf::Vector2f(xpos, ypos);
	transform.Transform.Scale = sf::Vector2f(0.05, 0.05);
	collider.Type = "Kill";
	collider.WrapToSprite = false;
	collider.Collider.width = 10;
	collider.Collider.height = 10;
	script.Bind<ScriptableLuaActor>();
	script.Scripts.push_back("assets/scripts/ProjectileMovement.lua");
	
	return proj;
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
		auto view = Registry.view<TagComponent, AttributesComponent, TransformComponent, CameraComponent>();
		for (auto actor : view)
		{
			auto [tag, attributes, transform, camera] = view.get<TagComponent, AttributesComponent, TransformComponent, CameraComponent>(actor);
			if (tag.Tag == "Player")
			{
				if (global::Game->DebugWindow)
				{
					ImGui::Begin("Debug Window");
					ImGui::DragInt("Lives", &global::Game->GameState.Lives);
					ImGui::Separator();
					ImGui::DragFloat("X", &transform.Transform.Pos.x);
					ImGui::DragFloat("Y", &transform.Transform.Pos.y);
					ImGui::Separator();
					ImGui::DragFloat("Gravity", &attributes.GetAttribute<float>("Gravity"));
					ImGui::DragFloat("Speed", &attributes.GetAttribute<float>("Speed"));
					ImGui::DragFloat("Jump Height", &attributes.GetAttribute<float>("JumpHeight"));
					ImGui::DragFloat("Max Y Speed", &attributes.GetAttribute<float>("MaxYSpeed"));
					ImGui::DragFloat("Kill Height", &attributes.GetAttribute<float>("KillHeight"));
					ImGui::Separator();
					ImGui::DragFloat("Camera Zoom", &camera.Zoom);
					ImGui::End();
					
				}
			}
		}
	}

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
					collider.Collider.width = spr.Sprite.getTexture()->getSize().x * transform.Transform.Scale.x;
					collider.Collider.height = spr.Sprite.getTexture()->getSize().y * transform.Transform.Scale.y;
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
				mainCamera->Camera.setSize(Renderer::GetView()->getSize().x * mainCamera->Zoom, Renderer::GetView()->getSize().y * mainCamera->Zoom);
			}
		
		auto group = Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto actor : group)
		{
			const auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(actor);
			sprite.Sprite.setPosition(transform.Transform.Pos);
			sprite.Sprite.setRotation(transform.Transform.Rot);
			sprite.Sprite.setScale(transform.Transform.Scale);
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
			tm.map.setScale(transform.Transform.Scale);
			tm.map.setRotation(transform.Transform.Rot);
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
	mainCamera->Camera.setSize(Renderer::GetView()->getSize().x * mainCamera->Zoom, Renderer::GetView()->getSize().y * mainCamera->Zoom);
	while (!Renderer::RenderQueue.empty())
		{
			Renderer::Draw(*Renderer::RenderQueue.top().elem, &mainCamera->Camera);
			Renderer::RenderQueue.pop();
		}
	}

	for (int i = 0; i < destroyList.size(); i++)
	{
		DestroyActor(Actor(destroyList[i], this));
	}
	destroyList.clear();
}


Actor Scene::CreateActor(const std::string& name)
{
	return CreateActorWithUUID(HPUUID(), name);
}

Actor Scene::CreateActorWithUUID(HPUUID uuid, const std::string& name)
{
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

Actor Scene::FindByName(std::string name)
{
	auto view = Registry.view<TagComponent>();
	for (auto& i : view)
	{
		auto& tag = view.get<TagComponent>(i);
		if(name == tag.Tag)
		{
			return Actor(i, this);
			
		}
	}
	return {};
}

GameState& Scene::GetGameState()
{
	return global::Game->GameState;
}

void Scene::OpenLevel(std::string path)
{
	global::Game->OpenLevel(path);
}

void Scene::DestroyActor(Actor actor)
{
	if (Registry.valid(actor))
		Registry.destroy(actor);
}

void Scene::AddToDestroyList(Actor actor)
{
	destroyList.push_back(actor);
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

template<>
void Scene::OnComponentAdded<AttributesComponent>(Actor actor, AttributesComponent& component)
{	
}