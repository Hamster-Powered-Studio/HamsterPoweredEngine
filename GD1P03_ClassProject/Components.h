#pragma once

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

#include "MetaStuff/Meta.h"


struct Transform2D {

	sf::Vector2f Pos = {0, 0};
	float Rot = 0;
	sf::Vector2f Scale = {1, 1};

	Transform2D() = default;
	Transform2D(const Transform2D& transform) : Pos(transform.Pos), Rot(transform.Rot), Scale(transform.Scale) {}
};

struct TransformComponent
{
	Transform2D Transform;
	
	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const Transform2D transform) : Transform(transform) {}

	operator Transform2D& () { return Transform; }
	operator const Transform2D& () const { return Transform; }
};

struct SpriteRendererComponent
{
	sf::Sprite Sprite;
	sf::Texture Texture;
	std::string Path;
	sf::Color Tint = sf::Color::White;
	bool Visible = true;

	SpriteRendererComponent() = default;
	SpriteRendererComponent(const SpriteRendererComponent&) = default;
	SpriteRendererComponent(const sf::Sprite other, const sf::Texture texture, const sf::String path,  const sf::Color tint) : Sprite(other), Texture(texture), Path(path), Tint(tint) {}
	SpriteRendererComponent(const std::string path) {
		Texture.loadFromFile(path);
		Path = path;
		Sprite.setOrigin(Texture.getSize().x / 2, Texture.getSize().y / 2);
		Sprite.setTexture(Texture, true);
	}

};

struct TagComponent
{
	std::string Tag;

	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	TagComponent(const std::string& tag) : Tag(tag) {}
};

struct CameraComponent
{
	sf::View Camera;
	float Zoom = 1.f;
	bool Primary = false;
	bool InheritRotation = false;

	CameraComponent() = default;
	CameraComponent(const CameraComponent&) = default;
	CameraComponent(const sf::View& camera) : Camera(camera) {}
};

struct InputComponent
{
	bool Active = true;

	InputComponent() = default;
	InputComponent(bool active) : Active(active) {};
	InputComponent(const InputComponent&) = default;
	//InputComponent(const sf::View & camera) : Camera(camera) {}
};

namespace meta
{
	template<>
	inline auto registerMembers<TransformComponent>()
	{
		return members(
			member("Transform", &TransformComponent::Transform)
		);
	}

	template<>
	inline auto registerMembers<Transform2D>()
	{
		return members(
			member("Position", &Transform2D::Pos),
			member("Rotation", &Transform2D::Rot),
			member("Scale", &Transform2D::Scale)
		);
	}

	template<>
	inline auto registerMembers<sf::Vector2f>()
	{
		return members(
			member("Position", &sf::Vector2f::x),
			member("Rotation", &sf::Vector2f::y)
		);
	}
}