#include "Tile.h"

#include <cassert>

#include <SFML/Graphics/Sprite.hpp>

#include "Game.h"


const sf::Vector2f BaseTile::TileSize = sf::Vector2f(16.0f, 16.0f);


BaseTile::BaseTile()
{
}


BaseTile::~BaseTile()
{
}


GenericTile::GenericTile(GenericTileType type) :
type_(type)
{
}


GenericTile::~GenericTile()
{
}


void GenericTile::Render(sf::RenderTarget& target, const sf::Vector2f& pos)
{
    sf::Sprite tileSprite(GameAssets::Get().genericTilesSheet);
	tileSprite.setPosition(pos);

	switch (type_) {
    case GenericTileType::RoomFloor:
        tileSprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
        break;

    case GenericTileType::RoomWall:
        tileSprite.setTextureRect(sf::IntRect(16, 0, 16, 16));
        break;

    case GenericTileType::PassageFloor:
        tileSprite.setTextureRect(sf::IntRect(32, 0, 16, 16));
        break;

    case GenericTileType::PassageWall:
        tileSprite.setTextureRect(sf::IntRect(48, 0, 16, 16));
        break;

	case GenericTileType::CaveFloor:
        tileSprite.setTextureRect(sf::IntRect(64, 0, 16, 16));
		break;

	case GenericTileType::CaveWall:
        tileSprite.setTextureRect(sf::IntRect(80, 0, 16, 16));
		break;
	}

	target.draw(tileSprite);
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

	case GenericTileType::CaveWall:
	case GenericTileType::RoomWall:
    case GenericTileType::PassageWall:
	default:
		return false;
	}
}
