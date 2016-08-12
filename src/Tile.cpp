#include "Tile.h"

#include <cassert>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

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
    if (Game::Get().IsInMapMode()) {
        // not rendering walls while in map mode
        // will save us some draw calls.
        if (!IsWalkable()) {
            return;
        }

        sf::RectangleShape tileRectangle(BaseTile::TileSize);
        tileRectangle.setPosition(pos);
        tileRectangle.setFillColor(type_ == GenericTileType::RoomFloorGold ? sf::Color(200, 150, 25) : sf::Color(100, 100, 100));

        target.draw(tileRectangle);
    }
    else {
        sf::Sprite tileSprite(GameAssets::Get().genericTilesSheet);
        tileSprite.setPosition(pos);

        switch (type_) {
        case GenericTileType::RoomFloor1:
            tileSprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
            break;

        case GenericTileType::RoomFloor2:
            tileSprite.setTextureRect(sf::IntRect(0, 16, 16, 16));
            break;

        case GenericTileType::RoomFloor3:
            tileSprite.setTextureRect(sf::IntRect(0, 32, 16, 16));
            break;

        case GenericTileType::RoomFloor4:
            tileSprite.setTextureRect(sf::IntRect(0, 48, 16, 16));
            break;

        case GenericTileType::RoomFloor5:
            tileSprite.setTextureRect(sf::IntRect(0, 64, 16, 16));
            break;

        case GenericTileType::RoomFloorGold:
            tileSprite.setTextureRect(sf::IntRect(0, 80, 16, 16));
            break;

        case GenericTileType::RoomWall:
        case GenericTileType::PassageWall:
            tileSprite.setTextureRect(sf::IntRect(16, 0, 16, 16));
            break;

        case GenericTileType::PassageFloor:
            tileSprite.setTextureRect(sf::IntRect(32, 0, 16, 16));
            break;

        case GenericTileType::CaveFloor:
            tileSprite.setTextureRect(sf::IntRect(64, 0, 16, 16));
            break;

        case GenericTileType::CaveWall:
            tileSprite.setTextureRect(sf::IntRect(80, 0, 16, 16));
            break;

        default:
            return;
        }

        target.draw(tileSprite);
    }
}


std::string GenericTile::GetName() const
{
	switch (type_) {
	case GenericTileType::CaveFloor:
		return "Cave Floor";

	case GenericTileType::CaveWall:
		return "Cave Wall";

	case GenericTileType::RoomFloor1:
    case GenericTileType::RoomFloor2:
    case GenericTileType::RoomFloor3:
    case GenericTileType::RoomFloor4:
    case GenericTileType::RoomFloor5:
    case GenericTileType::RoomFloorGold:
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
	case GenericTileType::RoomFloor1:
    case GenericTileType::RoomFloor2:
    case GenericTileType::RoomFloor3:
    case GenericTileType::RoomFloor4:
    case GenericTileType::RoomFloor5:
    case GenericTileType::RoomFloorGold:
	case GenericTileType::PassageFloor:
		return true;

	case GenericTileType::CaveWall:
	case GenericTileType::RoomWall:
    case GenericTileType::PassageWall:
	default:
		return false;
	}
}
