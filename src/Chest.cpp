#include "Chest.h"

#include <SFML/Graphics/Sprite.hpp>

#include "Game.h"
#include "Tile.h"
#include "Helper.h"


ChestEntity::ChestEntity(ChestType chestType, const std::string& chestFsNodeName) :
UnitEntity(),
chestType_(chestType),
isOpened_(false),
chestFsNodeName_(chestFsNodeName)
{
    SetSize(BaseTile::TileSize);
}


ChestEntity::~ChestEntity()
{
}


void ChestEntity::Use(EntityId playerId)
{
    // TODO: Give items

    if (!isOpened_) {
        isOpened_ = true;
    }
}


void ChestEntity::Render(sf::RenderTarget& target)
{
    sf::Sprite chestSprite(GameAssets::Get().chestsSpriteSheet);
    chestSprite.setPosition(GetPosition());

    if (!isOpened_) {
        // closed chest sprites
        switch (chestType_) {
        default:
        case ChestType::RedChest:
            chestSprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
            break;

        case ChestType::BlueChest:
            chestSprite.setTextureRect(sf::IntRect(0, 16, 16, 16));
            break;

        case ChestType::PurpleChest:
            chestSprite.setTextureRect(sf::IntRect(0, 32, 16, 16));
            break;
        }
    }
    else {
        // open chest sprites
        switch (chestType_) {
        default:
        case ChestType::RedChest:
            chestSprite.setTextureRect(sf::IntRect(16, 0, 16, 16));
            break;

        case ChestType::BlueChest:
            chestSprite.setTextureRect(sf::IntRect(16, 16, 16, 16));
            break;

        case ChestType::PurpleChest:
            chestSprite.setTextureRect(sf::IntRect(16, 32, 16, 16));
            break;
        }
    }

    target.draw(chestSprite);

    if (!chestFsNodeName_.empty()) {
        auto area = GetAssignedArea();

        if (area) {
            auto chestText = std::make_unique<sf::Text>(chestFsNodeName_, GameAssets::Get().gameFont, 18);
            chestText->setScale(0.15f, 0.15f);
            chestText->setColor(isOpened_ ? sf::Color(150, 150, 150) : sf::Color(255, 165, 0));

            auto textPos = sf::Vector2f(GetCenterPosition().x, GetPosition().y) -
                sf::Vector2f(0.5f * chestText->getGlobalBounds().width, 3.0f);

            chestText->setPosition(textPos);

            area->AddFrameUIRenderable(Helper::GetTextDropShadow(*chestText, sf::Vector2f(0.5f, 0.5f)));
            area->AddFrameUIRenderable(chestText);
        }
    }
}