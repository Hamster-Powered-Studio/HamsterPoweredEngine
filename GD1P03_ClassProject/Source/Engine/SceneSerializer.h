#pragma once
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
	bool DeserializeRuntime(const std::string& filepath);
private:
	void SerializeActor(YAML::Emitter& out, Actor actor);
	Scene& m_Scene;
};

