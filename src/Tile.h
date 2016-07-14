#pragma once

#include <string>

#include <SFML/Graphics/RenderTarget.hpp>

/**
* Represents the base class of a tile inside of the game world.
*/
class BaseTile
{
public:
    static const sf::Vector2f TileSize;

	BaseTile();
	virtual ~BaseTile();

	virtual void Tick() = 0;
	virtual void Render(sf::RenderTarget& target, const sf::Vector2f& pos) = 0;

	virtual std::string GetName() const = 0;
	virtual bool IsWalkable() const = 0;
};

/**
* The different types of generic tiles.
*/
enum class GenericTileType
{
	RoomFloor,
	RoomWall,
	PassageFloor,
    PassageWall,
	CaveFloor,
	CaveWall,
	Pillar
};

/**
* Represents a basic static tile.
*/
class GenericTile : public BaseTile
{
	GenericTileType type_;

public:
	GenericTile(GenericTileType type);
	virtual ~GenericTile();

	inline virtual void Tick() override { }
	virtual void Render(sf::RenderTarget& target, const sf::Vector2f& pos) override;

	inline virtual GenericTileType GetType() const { return type_; }

	virtual std::string GetName() const override;
	virtual bool IsWalkable() const override;
};