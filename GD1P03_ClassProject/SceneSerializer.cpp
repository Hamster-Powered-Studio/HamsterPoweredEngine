#include "SceneSerializer.h"

#include "yaml-cpp/yaml.h"
#include <fstream>
#include "Components.h"
#include "Actor.h"
#include <iostream>
#include "MetaStuff/Meta.h"

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
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<sf::Color>
	{
		static Node encode(const sf::Color& rhs)
		{
			Node node;
			node.push_back(rhs.toInteger());
			return node;
		}

		static bool decode(const Node& node, sf::Color& rhs)
		{
			rhs = sf::Color(node.as<sf::Uint32>());
			return true;
		}
	};
}

YAML::Emitter& operator<<(YAML::Emitter& out, const sf::Vector2f& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
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


static void SerializeActor(YAML::Emitter& out, Actor actor)
{
	out << YAML::BeginMap;
	out << YAML::Key << "Actor";
	out << YAML::Key << "12345"; //ActorID


	if (actor.HasComponent<TagComponent>())
	{
		out << YAML::Key << "TagComponent";
		out << YAML::BeginMap;

		auto& tag = actor.GetComponent<TagComponent>().Tag;
		out << YAML::Key << "Tag" << YAML::Value << tag;

		out << YAML::EndMap;
	}

	if (actor.HasComponent<TransformComponent>())
	{
		out << YAML::Key << "TransformComponent";
		out << YAML::BeginMap;

		auto& transform = actor.GetComponent<TransformComponent>();
		out << YAML::Key << "Position" << YAML::Value << transform.Transform.Pos;
		out << YAML::Key << "Rotation" << YAML::Value << transform.Transform.Rot;
		out << YAML::Key << "Scale" << YAML::Value << transform.Transform.Scale;
		

		out << YAML::EndMap;

	}

	if (actor.HasComponent<CameraComponent>())
	{
		out << YAML::Key << "CameraComponent";
		out << YAML::BeginMap;

		auto& cam = actor.GetComponent<CameraComponent>();
		out << YAML::Key << "Zoom" << YAML::Value << cam.Zoom;
		out << YAML::Key << "Primary" << YAML::Value << cam.Primary;
		out << YAML::Key << "InheritRot" << YAML::Value << cam.InheritRotation;


		out << YAML::EndMap;
	}

	if (actor.HasComponent<SpriteRendererComponent>())
	{
		out << YAML::Key << "SpriteRendererComponent";
		out << YAML::BeginMap;

		auto& sprite = actor.GetComponent<SpriteRendererComponent>();
		out << YAML::Key << "Texture" << YAML::Value << sprite.Path;
		out << YAML::Key << "Tint" << YAML::Value << sprite.Tint.toInteger();
		out << YAML::Key << "Visible" << YAML::Value << sprite.Visible;


		out << YAML::EndMap;
	}

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
				tc.Transform.Pos = transformComponent["Position"].as<sf::Vector2f>();
				tc.Transform.Rot = transformComponent["Rotation"].as<float>();
				tc.Transform.Scale = transformComponent["Scale"].as<sf::Vector2f>();
			}

			auto cameraComponent = actor["CameraComponent"];
			if (cameraComponent)
			{
				auto& cam = deserializedActor.AddComponent<CameraComponent>();
				cam.Zoom = cameraComponent["Zoom"].as<float>();
				cam.Primary = cameraComponent["Primary"].as<bool>();
				cam.InheritRotation = cameraComponent["InheritRot"].as<bool>();
			}

			auto spriteComponent = actor["SpriteRendererComponent"];
			if (spriteComponent)
			{
				auto& sprite = deserializedActor.GetComponent<SpriteRendererComponent>();
				sprite.Path = spriteComponent["Texture"].as<std::string>();
				sprite.Visible = spriteComponent["Visible"].as<bool>();
				sprite.Tint = spriteComponent["Tint"].as<sf::Color>();
				sprite.Texture.loadFromFile(sprite.Path);
				sprite.Sprite.setTexture(sprite.Texture, true);
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

