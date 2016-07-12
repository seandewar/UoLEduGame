#include "Tile.h"

#include <cassert>

#include <SFML/Graphics/RectangleShape.hpp>


GenericTile::GenericTile(GenericTileType type) :
type_(type)
{
}


GenericTile::~GenericTile()
{
}


void GenericTile::Render(sf::RenderTarget& target, const sf::Vector2f& pos)
{
	// TODO use a sprite
	sf::RectangleShape tile;
	tile.setPosition(pos);
	tile.setSize(sf::Vector2f(TILE_WIDTH, TILE_HEIGHT));

	switch (type_) {
	case GenericTileType::CaveFloor:
		tile.setFillColor(sf::Color(115, 77, 38));
		break;

	case GenericTileType::CaveWall:
		tile.setFillColor(sf::Color(77, 51, 25));
		break;

	case GenericTileType::PassageFloor:
		tile.setFillColor(sf::Color(100, 100, 100));
		break;

    case GenericTileType::PassageWall:
        tile.setFillColor(sf::Color(50, 50, 50));
        break;

	case GenericTileType::RoomFloor:
		tile.setFillColor(sf::Color(95, 57, 18));
		break;

	case GenericTileType::RoomWall:
		tile.setFillColor(sf::Color(55, 55, 55));
		break;

	case GenericTileType::Pillar:
		tile.setFillColor(sf::Color(100, 100, 100));
		break;
	}

	target.draw(tile);
}


std::string GenericTile::GetName() const
{
	switch (type_) {
	case GenericTileType::CaveFloor:
		return "Cave Floor";

	case GenericTileType::CaveWall:
		return "Cave Wall";

	case GenericTileType::RoomFloor:
		return "Room Floor";

	case GenericTileType::PassageFloor:
		return "Passage Floor";

    case GenericTileType::PassageWall:
        return "Passage Wall";

	case GenericTileType::RoomWall:
		return "Room Wall";

	case GenericTileType::Pillar:
		return "Pillar";

	default:
		return "GenericTile";
	}
}


bool GenericTile::IsWalkable() const
{
	switch (type_) {
	case GenericTileType::CaveFloor:
	case GenericTileType::RoomFloor:
	case GenericTileType::PassageFloor:
		return true;

	case GenericTileType::Pillar:
	case GenericTileType::CaveWall:
	case GenericTileType::RoomWall:
    case GenericTileType::PassageWall:
	default:
		return false;
	}
}


BaseTile::BaseTile()
{
}


BaseTile::~BaseTile()
{
}
