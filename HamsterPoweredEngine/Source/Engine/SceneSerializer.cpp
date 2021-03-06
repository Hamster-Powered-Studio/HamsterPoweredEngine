#include "SceneSerializer.h"

#include "yaml-cpp/yaml.h"
#include <fstream>
#include "Components/Components.h"
#include "Actor.h"
#include <iostream>
#include "MetaStuff/Meta.h"
#include <algorithm>
#include <SFML/OpenGL.hpp>

#include "Global.h"

namespace YAML {

	template<>
	struct convert<sf::Vector2f>
	{
		static Node encode(const sf::Vector2f& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, sf::Vector2f& rhs)
		{
			rhs.x = node["x"].as<float>();
			rhs.y = node["y"].as<float>();
			return true;
		}
	};

		template<>
	struct convert<HPUUID>
		{
			static Node encode(const HPUUID& rhs)
			{
				Node node;
				node.push_back(rhs);
				return node;
			}

			static bool decode(const Node& node, HPUUID& rhs)
			{
				rhs = node["ID"].as<uint64_t>();
				return true;
			}
		};

	template<>
	struct convert<std::unordered_map<std::string, std::variant<int, float, std::string>>>
		{
			static Node encode(const std::unordered_map<std::string, std::variant<int, float, std::string>>& rhs)
			{
				Node node;
				
				return node;
			}

			static bool decode(const Node& node, std::unordered_map<std::string, std::variant<int, float, std::string>>& rhs)
			{
				for (auto& att : node)
				{
					std::string name = att.first.as<std::string>();
					const YAML::Node& value = att.second;
					int type = value["Type"].as<int>();
					std::variant<int, float, std::string> avalue;
					switch (type)
					{
					case 0:
						avalue = value["Value"].as<int>();
						break;
					case 1:
						avalue = value["Value"].as<float>();
						break;
					case 2:
						avalue = value["Value"].as<std::string>();
						break;
					}
					
					rhs[name] = avalue;
					
				}
				
				return true;
			}
		};
	
	template<>
	struct convert<sf::Color>
	{
		static Node encode(const sf::Color& rhs)
		{
			Node node;
			//node.push_back(rhs.toInteger());
			return node;
		}

		static bool decode(const Node& node, sf::Color& rhs)
		{
			rhs = sf::Color(node.as<sf::Uint32>());
			return true;
		}
	};



}

template<typename T>
YAML::Emitter& operator<<(YAML::Emitter& out, const T& Class)
{
	out << YAML::BeginMap;
	meta::doForAllMembers<T>([&out, &Class](const auto& member)
	{
		out << YAML::Key << member.getName() << YAML::Value << member.get(Class);
	});
	out << YAML::EndMap;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const HPUUID& id)
{
	std::ostringstream oss;
	oss << id;
	out << oss.str();
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const std::unordered_map<std::string, std::variant<int, float, std::string>>& map)
{
	out << YAML::BeginMap;
	for (auto& i : map)
	{
		out << YAML::Key << i.first << YAML::Value;
		out << YAML::BeginMap;
		out << YAML::Key << "Type" << YAML::Value << i.second.index();
		
		switch (i.second.index())
		{
		case 0:
			out << YAML::Key << "Value" << YAML::Value << std::get<0>(i.second);
			break;
		case 1:
			out << YAML::Key << "Value" << YAML::Value << std::get<1>(i.second);
			break;
		case 2:
			out << YAML::Key << "Value" << YAML::Value << std::get<2>(i.second);
			break;
		default:
			out << YAML::Key << "Value" << YAML::Value << "Broken";
			break;
		}
		out << YAML::EndMap;
		
		
		
	}
	out << YAML::EndMap;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const Actor& actor)
{
	std::ostringstream oss;
	HPUUID id = actor.GetUUID();
	oss << id;
	out << oss.str();
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const sf::Color& v)
{
	out << v.toInteger();
	return out;
}

SceneSerializer::SceneSerializer(Scene& scene) : m_Scene(scene)
{
}

static void RemoveWordFromLine(std::string& line, const std::string word)
{
	line = line.substr(line.find_first_of(word) + 1);
}

template<typename T>
static void SerializeComponent(YAML::Emitter& out, Actor* actor)
{
	if (actor)
	{
		if (actor->HasComponent<T>())
		{
			auto& comp = actor->GetComponent<T>();
			std::string type = typeid(comp).name();
			RemoveWordFromLine(type, " ");
			out << YAML::Key << type;
			out << comp;
		}
	}
}


void SceneSerializer::SerializeActor(YAML::Emitter& out, Actor actor)
{
	if (actor.m_ActorHandle != entt::null)
	{
		out << YAML::BeginMap;

		out << YAML::Key << "Actor" << YAML::Value << actor.GetUUID();

		SerializeComponent<IDComponent>(out, &actor);
		SerializeComponent<TagComponent>(out, &actor);
		SerializeComponent<TransformComponent>(out, &actor);
		SerializeComponent<RelationshipComponent>(out, &actor);
		SerializeComponent<LuaScriptComponent>(out, &actor);
		SerializeComponent<CameraComponent>(out, &actor);
		SerializeComponent<SpriteRendererComponent>(out, &actor);
		SerializeComponent<TileMapComponent>(out, &actor);
		SerializeComponent<BoxColliderComponent>(out, &actor);
		SerializeComponent<AttributesComponent>(out, &actor);
		out << YAML::EndMap;
	}
	else out << "null";
}


void SceneSerializer::Serialize(const std::string& filepath)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene";
	out << YAML::Value << "Untitled";
	out << YAML::Key << "Actors";
	out << YAML::Value << YAML::BeginSeq;
	m_Scene.Registry.each([&](auto actorID)
		{
			Actor actor = { actorID, m_Scene.get() };
			if (!actor)
				return;

			SerializeActor(out, actor);
		});
	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
}

void SceneSerializer::SerializeRuntime(const std::string& filepath)
{
	assert(false);
}

bool SceneSerializer::Deserialize(const std::string& filepath)
{
	std::ifstream stream(filepath);
	std::stringstream strStream;
	strStream << stream.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());
	if(!data["Scene"])
		return false;

	std::cout << "Deserializing" << std::endl;
	std::string sceneName = data["Scene"].as<std::string>();

	auto actors = data["Actors"];
	if (actors)
	{
		std::vector<Actor> actorList;
		for (auto actor : actors)
		{
			actorList.push_back(DeserializeActor(actor));
		}
		for (auto actor : actorList)
		{
			auto& rel = actor.GetComponent<RelationshipComponent>();
			if (rel.tempParentUUID != 0)
				rel.Parent = global::Game->currentScene->GetByUUID(rel.tempParentUUID);
			for (auto child : rel.tempChildUUIDs)
				rel.children.push_back(global::Game->currentScene->GetByUUID(child));
		}
	}
	else std::cout << "No actors";
	return true;
}

Actor SceneSerializer::DeserializeActor(YAML::detail::iterator_value& actor)
{
	uint64_t uuid = actor["Actor"].as<uint64_t>();
			
			std::string name;
			auto tagComponent = actor["TagComponent"];
			if (tagComponent)
			{
				name = tagComponent["Tag"].as<std::string>();
			}
				

			Actor deserializedActor = m_Scene.CreateActorWithUUID(uuid, name);
			
			auto transformComponent = actor["TransformComponent"];
			if (transformComponent)
			{
				auto& tc = deserializedActor.GetComponent<TransformComponent>();
				tc.Transform.Pos = transformComponent["Transform"]["Position"].as<sf::Vector2f>();
				tc.Transform.Rot = transformComponent["Transform"]["Rotation"].as<float>();
				tc.Transform.Scale = transformComponent["Transform"]["Scale"].as<sf::Vector2f>();
			}

			auto relationshipComponent = actor["RelationshipComponent"];
			if (relationshipComponent)
			{
				auto& input = deserializedActor.GetComponent<RelationshipComponent>();
				input.tempParentUUID = relationshipComponent["Parent"].as<uint64_t>();
				input.tempChildUUIDs = relationshipComponent["Children"].as<std::vector<uint64_t>>();
				input.Attached = relationshipComponent["Attached"].as<bool>();
				input.Offset.Pos = relationshipComponent["Offset"]["Position"].as<sf::Vector2f>();
				input.Offset.Rot = relationshipComponent["Offset"]["Rotation"].as<float>();
				input.Offset.Scale = relationshipComponent["Offset"]["Scale"].as<sf::Vector2f>();
			}

			auto cameraComponent = actor["CameraComponent"];
			if (cameraComponent)
			{
				auto& cam = deserializedActor.AddComponent<CameraComponent>();
				cam.Zoom = cameraComponent["Zoom"].as<float>();
				cam.Primary = cameraComponent["Primary"].as<bool>();
				cam.InheritRotation = cameraComponent["InheritRotation"].as<bool>();
			}

			auto spriteComponent = actor["SpriteRendererComponent"];
			if (spriteComponent)
			{
				auto& sprite = deserializedActor.AddComponent<SpriteRendererComponent>(spriteComponent["Texture"].as<std::string>());
				sprite.Path = spriteComponent["Texture"].as<std::string>();
				sprite.Visible = spriteComponent["Visible"].as<bool>();
				sprite.Tint = spriteComponent["Tint"].as<sf::Color>();
				//sprite.Texture.loadFromFile(sprite.Path);
				sprite.Sprite.setTexture(sprite.Texture, true);
				if(spriteComponent["ZOrder"]) sprite.ZOrder = spriteComponent["ZOrder"].as<float>();
			}

			auto tmComponent = actor["TileMapComponent"];
			if (tmComponent)
			{
				auto& input = deserializedActor.AddComponent<TileMapComponent>();
				input.Layout = tmComponent["Layout"].as<std::vector<int>>();
				input.height = tmComponent["Height"].as<int>();
				input.width = tmComponent["Width"].as<int>();
				input.Path = tmComponent["TexturePath"].as<std::string>();
				input.tileWidth = tmComponent["TileWidth"].as<int>();
				input.tileHeight = tmComponent["TileHeight"].as<int>();
				if(tmComponent["Visible"]) input.Visible = tmComponent["Visible"].as<bool>();
				if(tmComponent["ZOrder"]) input.ZOrder = tmComponent["ZOrder"].as<float>();
				if(tmComponent["Colliders"]) input.Colliders = tmComponent["Colliders"].as<std::vector<int>>();
				input.Load();
			}

			auto box = actor["BoxColliderComponent"];
			if (box)
			{
				auto& input = deserializedActor.AddComponent<BoxColliderComponent>();
				input.Active = box["Active"].as<bool>();
				input.Preview = box["Preview"].as<bool>();
				input.WrapToSprite = box["Preview"].as<bool>();
				input.Offset = box["Offset"].as<sf::Vector2f>();
				input.Collider.height = box["Collider"]["Height"].as<float>();
				input.Collider.width = box["Collider"]["Width"].as<float>();
				input.Collider.top = box["Collider"]["Top"].as<float>();
				input.Collider.left = box["Collider"]["Left"].as<float>();
				if(box["Type"]) input.Type = box["Type"].as<std::string>();
			}

			auto lua = actor["LuaScriptComponent"];
			if (lua)
			{
				auto& input = deserializedActor.AddComponent<LuaScriptComponent>();
				input.Bind<ScriptableLuaActor>();
				input.Scripts = lua["Scripts"].as<std::vector<std::string>>();
			}

			auto att = actor["AttributesComponent"];
			if (att)
			{
				auto& input = deserializedActor.AddComponent<AttributesComponent>();
				input.Attributes = att["Attributes"].as<std::unordered_map<std::string, std::variant<int, float, std::string>>>();
			}
	return deserializedActor;
}

bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
{
	assert(false);
	return false;
}

