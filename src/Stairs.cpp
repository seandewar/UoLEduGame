#include "Stairs.h"

#include <SFML/Graphics/Sprite.hpp>

#include "Game.h"
#include "Helper.h"


StairEntity::StairEntity() :
UnitEntity()
{
    SetSize(BaseTile::TileSize);
}


StairEntity::~StairEntity()
{
}


UpStairEntity::UpStairEntity() :
StairEntity()
{
    SetSize(BaseTile::TileSize);
}


UpStairEntity::~UpStairEntity()
{
}


void UpStairEntity::Render(sf::RenderTarget& target)
{
    sf::Sprite stairSprite(GameAssets::Get().stairsSpriteSheet, sf::IntRect(16, 0, 16, 16));
    stairSprite.setPosition(GetPosition());
    target.draw(stairSprite);

    auto area = GetAssignedArea();

    if (area) {
        auto stairTargetText = std::make_unique<sf::Text>("..", GameAssets::Get().gameFont, 18);
        stairTargetText->setScale(0.15f, 0.15f);
        stairTargetText->setColor(sf::Color(255, 255, 0));

        auto textPos = sf::Vector2f(GetCenterPosition().x, GetPosition().y) -
            sf::Vector2f(0.5f * stairTargetText->getGlobalBounds().width, 4.0f);

        stairTargetText->setPosition(textPos);

        area->AddFrameUIRenderable(Helper::GetTextDropShadow(*stairTargetText, sf::Vector2f(0.5f, 0.5f)));
        area->AddFrameUIRenderable(stairTargetText);
    }
}


void UpStairEntity::Use(EntityId playerId)
{
    auto area = GetAssignedArea();

    if (area) {
        auto areaFsNode = area->GetRelatedNode();

        if (areaFsNode) {
            auto targetFsNode = areaFsNode->GetParent();

            if (targetFsNode) {
                auto newFsNodePath = GameFilesystem::GetNodePathString(*targetFsNode);
                Game::Get().SetLevelChange(newFsNodePath);
            }
        }
    }
}


DownStairEntity::DownStairEntity(const std::string& destinationFsNodeName) :
StairEntity(),
destinationFsNodeName_(destinationFsNodeName)
{
}


DownStairEntity::~DownStairEntity()
{
}


void DownStairEntity::Render(sf::RenderTarget& target)
{
    sf::Sprite stairSprite(GameAssets::Get().stairsSpriteSheet, sf::IntRect(0, 0, 16, 16));
    stairSprite.setPosition(GetPosition());
    target.draw(stairSprite);

    if (!destinationFsNodeName_.empty()) {
        auto area = GetAssignedArea();

        if (area) {
            auto stairTargetText = std::make_unique<sf::Text>(destinationFsNodeName_, GameAssets::Get().gameFont, 18);
            stairTargetText->setScale(0.15f, 0.15f);
            stairTargetText->setColor(sf::Color(255, 255, 0));

            auto textPos = sf::Vector2f(GetCenterPosition().x, GetPosition().y) -
                sf::Vector2f(0.5f * stairTargetText->getGlobalBounds().width, 4.0f);

            stairTargetText->setPosition(textPos);

            area->AddFrameUIRenderable(Helper::GetTextDropShadow(*stairTargetText, sf::Vector2f(0.5f, 0.5f)));
            area->AddFrameUIRenderable(stairTargetText);
        }
    }
}


void DownStairEntity::Use(EntityId playerId)
{
    auto area = GetAssignedArea();

    if (area) {
        auto areaFsNode = area->GetRelatedNode();

        if (areaFsNode) {
            auto targetFsNode = areaFsNode->GetChildNode(destinationFsNodeName_);

            if (targetFsNode) {
                auto newFsNodePath = GameFilesystem::GetNodePathString(*targetFsNode);
                Game::Get().SetLevelChange(newFsNodePath);
            }
        }
    }
}