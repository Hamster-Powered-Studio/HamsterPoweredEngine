#pragma once
#include <yaml-cpp/node/iterator.h>

#include "Scene.h"
namespace YAML
{
	class Emitter;
}

class SceneSerializer
{
public:
	SceneSerializer(Scene& scene);

	void Serialize(const std::string& filepath);
	void SerializeRuntime(const std::string& filepath);

	bool Deserialize(const std::string& filepath);
	Actor DeserializeActor(YAML::detail::iterator_value& actor);
	bool DeserializeRuntime(const std::string& filepath);

	static void SerializeActor(YAML::Emitter& out, Actor actor);
private:
	friend struct Actor;
	Scene& m_Scene;
};

