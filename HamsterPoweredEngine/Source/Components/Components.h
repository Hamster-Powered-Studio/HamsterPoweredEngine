#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "MetaStuff/Meta.h"
#include "Engine/TileMap.h"
#include "Engine/HPUUID.h"
#include "Engine/Actor.h"


struct Transform2D {

	sf::Vector2f Pos = {0, 0};
	float Rot = 0;
	sf::Vector2f Scale = {1, 1};

	Transform2D() = default;
	Transform2D(const Transform2D& transform) : Pos(transform.Pos), Rot(transform.Rot), Scale(transform.Scale) {}

	Transform2D operator+ (const Transform2D& other)
	{
		Transform2D newTransform;
		newTransform.Pos = Pos + other.Pos;
		newTransform.Rot = Rot + other.Rot;
		newTransform.Scale = {Scale.x * other.Scale.x, Scale.y * other.Scale.y};
		
		return newTransform;
	}
};

struct IDComponent
{
	HPUUID ID;
	IDComponent() = default;
	IDComponent(const IDComponent&) = default;
	IDComponent(const HPUUID& id) : ID(id){}
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
	std::string Path = "assets/sprites/DefaultSprite.png";
	sf::Color Tint = sf::Color::White;
	float ZOrder = 0;
	bool Visible = true;

	SpriteRendererComponent()
	{
		Texture.loadFromFile(Path);
		Sprite.setOrigin(Texture.getSize().x / 2, Texture.getSize().y / 2);
		Sprite.setTexture(Texture, true);
	}
	SpriteRendererComponent(const SpriteRendererComponent&) = default;
	SpriteRendererComponent(const sf::Sprite other, const sf::Texture texture, const sf::String path,  const sf::Color tint) : Sprite(other), Texture(texture), Path(path), Tint(tint) {}
	SpriteRendererComponent(const std::string path) {
		Texture.loadFromFile(path);
		Path = path;
		Sprite.setOrigin(Texture.getSize().x / 2, Texture.getSize().y / 2);
		Sprite.setTexture(Texture, true);
	}

	//Overload the < operator.
	bool operator< (const SpriteRendererComponent& Spr1)
	{
		return ZOrder > Spr1.ZOrder;
	}
	//Overload the > operator.
	bool operator> (const SpriteRendererComponent& Spr1)
	{
		return ZOrder < Spr1.ZOrder;
	}
};

struct TagComponent
{
	std::string Tag;
	TagComponent() = default;
	TagComponent(const TagComponent&) = default;
	TagComponent(const std::string& tag) : Tag(tag) {}
};

struct RelationshipComponent
{
	Actor Parent;
	std::vector<Actor> children = {};
	HPUUID tempParentUUID = 0;
	std::vector<uint64_t> tempChildUUIDs;
	bool Attached = true;
	Transform2D Offset;

	RelationshipComponent() = default;
	RelationshipComponent(const RelationshipComponent&) = default;
};

struct CameraComponent
{
	sf::View Camera;
	float Zoom = 1.f;
	bool Primary = false;
	bool InheritRotation = false;

	bool GetPrimary() const { return Primary; }
	void SetPrimary(bool primary) { Primary = primary; }

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

struct TileMapComponent
{
	TileMap map;
	std::string Path = "assets/sprites/RPGpack_sheet.png";
	sf::Texture Texture;
	int tileWidth = 64;
	int tileHeight = 64;
	int width = 50;
	int height = 50;
	int SelectedTile = 0;
	int MaxTilesX = 0;
	float ZOrder = 0;
	bool Visible = true;
	std::vector<int> Layout;

	TileMapComponent()
	{
		Load();
	}
	TileMapComponent(const TileMapComponent&) = default;

	//Overload the < operator.
	bool operator< (const TileMapComponent &Spr2)
	{
		return ZOrder > Spr2.ZOrder;
	}
	//Overload the > operator.
	bool operator> (const TileMapComponent &Spr2)
	{
		return ZOrder < Spr2.ZOrder;
	}
	
	void Load()
	{
		Texture.loadFromFile(Path);
		MaxTilesX = static_cast<int>(std::floor(static_cast<double>(Texture.getSize().x / tileWidth)/static_cast<double>(1))*static_cast<double>(1));

		if (!Layout.empty())
		{
			//load layout
			Layout.resize(width * height);
		}
		else
		{
			Layout.resize(width * height);
			for (auto layout : Layout)
			{
				layout = 0;
			}
		}
		map.load(Path, sf::Vector2u(tileWidth, tileHeight), Layout, width, height);
	}
	void UpdateTexture(int tileToChange)
	{
		//map.updateTileTexture(Layout, SelectedTile, tileToChange, sf::Vector2f(tileWidth, tileHeight));
		map.updateTileTexture(Layout, SelectedTile, tileToChange, sf::Vector2u(tileWidth, tileHeight));
	}
};

namespace meta
{
	template<>
	inline auto registerMembers<IDComponent>()
	{
		return members(
			member("ID", &IDComponent::ID)
		);
	}
	
	template<>
	inline auto registerMembers<TransformComponent>()
	{
		return members(
			member("Transform", &TransformComponent::Transform)
		);
	}

	template<>
	inline auto registerMembers<TagComponent>()
	{
		return members(
			member("Tag", &TagComponent::Tag)
		);
	}

	template<>
	inline auto registerMembers<CameraComponent>()
	{
		return members(
			member("Zoom", &CameraComponent::Zoom),
			member("Primary", &CameraComponent::Primary),
			member("InheritRotation", &CameraComponent::InheritRotation)
		);
	}

	template<>
	inline auto registerMembers<InputComponent>()
	{
		return members(
			member("Active", &InputComponent::Active)
		);
	}

	template<>
	inline auto registerMembers<SpriteRendererComponent>()
	{
		return members(
			member("Texture", &SpriteRendererComponent::Path),
			member("Tint", &SpriteRendererComponent::Tint),
			member("Visible", &SpriteRendererComponent::Visible),
			member("ZOrder", &SpriteRendererComponent::ZOrder)
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
			member("x", &sf::Vector2f::x),
			member("y", &sf::Vector2f::y)
		);
	}

	template<>
	inline auto registerMembers<TileMapComponent>()
	{
		return members(
			member("TexturePath", &TileMapComponent::Path),
			member("Layout", &TileMapComponent::Layout),
			member("TileWidth", &TileMapComponent::tileWidth),
			member("TileHeight", &TileMapComponent::tileHeight),
			member("Width", &TileMapComponent::width),
			member("Height", &TileMapComponent::height),
			member("ZOrder", &TileMapComponent::ZOrder),
			member("Visible", &TileMapComponent::Visible)
		);
	}

	template<>
	inline auto registerMembers<RelationshipComponent>()
	{
		return members(
			member("Parent", &RelationshipComponent::Parent),
			member("Children", &RelationshipComponent::children),
			member("Attached", &RelationshipComponent::Attached),
			member("Offset", &RelationshipComponent::Offset)
		);
	}
}