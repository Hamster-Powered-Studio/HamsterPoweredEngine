#include "SceneSerializer.h"

#include "yaml-cpp/yaml.h"
#include <fstream>
#include "Components/Components.h"
#include "Actor.h"
#include <iostream>
#include "MetaStuff/Meta.h"
#include <algorithm>

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
			//if (node.IsSequence() || node.size() != 2)
			//	return false;

			//auto props = node["Actors"]
			rhs.x = node["x"].as<float>();
			rhs.y = node["y"].as<float>();
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
/*
	template<>
	struct convert<std::vector<int>>
		{
			static Node encode(const std::vector<int>& rhs)
			{
				Node node;
				for (auto i : rhs)
				{
					node.push_back(i);
				}
				return node;
			}

			static bool decode(const Node& node, std::vector<int>& rhs)
			{
				if (node.IsSequence())
					return false;

				
				rhs.emplace_back(node.as<int>());
				return true;
			}
		};
		*/
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

//YAML::Emitter& operator<<(YAML::Emitter& out, const sf::Vector2f& v)
//{
//	out << YAML::Flow;
//	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
//	return out;
//}

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
static void SerializeComponent(YAML::Emitter& out, Actor& actor)
{
	if (actor.HasComponent<T>())
	{
		auto& comp = actor.GetComponent<T>();
		std::string type = typeid(comp).name();
		RemoveWordFromLine(type, " ");
		out << YAML::Key << type;
		out << comp;
	}
}

void SceneSerializer::SerializeActor(YAML::Emitter& out, Actor actor)
{
	out << YAML::BeginMap;
	out << YAML::Key << "Actor";
	out << YAML::Key << "12345"; //ActorID

	SerializeComponent<TagComponent>(out, actor);
	SerializeComponent<TransformComponent>(out, actor);
	SerializeComponent<CameraComponent>(out, actor);
	SerializeComponent<SpriteRendererComponent>(out, actor);
	SerializeComponent<InputComponent>(out, actor);
	SerializeComponent<TileMapComponent>(out, actor);
	out << YAML::EndMap;
	
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
		for (auto actor : actors)
		{
			uint64_t uuid = actor["Actor"].as<uint64_t>(); //TODO

			std::string name;
			auto tagComponent = actor["TagComponent"];
			if (tagComponent)
				name = tagComponent["Tag"].as<std::string>();

			Actor deserializedActor = m_Scene.CreateActor(name);

			auto transformComponent = actor["TransformComponent"];
			if (transformComponent)
			{
				auto& tc = deserializedActor.GetComponent<TransformComponent>();
				tc.Transform.Pos = transformComponent["Transform"]["Position"].as<sf::Vector2f>();
				tc.Transform.Rot = transformComponent["Transform"]["Rotation"].as<float>();
				tc.Transform.Scale = transformComponent["Transform"]["Scale"].as<sf::Vector2f>();
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
				auto& sprite = deserializedActor.AddComponent<SpriteRendererComponent>();
				sprite.Path = spriteComponent["Texture"].as<std::string>();
				sprite.Visible = spriteComponent["Visible"].as<bool>();
				sprite.Tint = spriteComponent["Tint"].as<sf::Color>();
				sprite.Texture.loadFromFile(sprite.Path);
				sprite.Sprite.setTexture(sprite.Texture, true);
				if(spriteComponent["ZOrder"]) sprite.ZOrder = spriteComponent["ZOrder"].as<float>();
			}

			auto inputComponent = actor["InputComponent"];
			if (inputComponent)
			{
				auto& input = deserializedActor.AddComponent<InputComponent>();
				input.Active = inputComponent["Active"].as<bool>();
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
				if(tmComponent["ZOrder"]) input.ZOrder = tmComponent["ZOrder"].as<float>();
				input.Load();
			}
		}
	}
	else std::cout << "No actors";
	return true;
}

bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
{
	assert(false);
	return false;
}

