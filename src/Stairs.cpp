#include "Stairs.h"

#include <SFML/Graphics/Sprite.hpp>

#include "Game.h"
#include "Helper.h"


StairEntity::StairEntity() :
UnitEntity()
{
    SetSize(sf::Vector2f(16.0f, 16.0f));
}


StairEntity::~StairEntity()
{
}


UpStairEntity::UpStairEntity() :
StairEntity()
{
    SetSize(sf::Vector2f(16.0f, 16.0f));
}


UpStairEntity::~UpStairEntity()
{
}


void UpStairEntity::Render(sf::RenderTarget& target)
{
    sf::Sprite stairSprite(GameAssets::Get().stairsSpriteSheet, sf::IntRect(16, 0, 16, 16));
    stairSprite.setPosition(GetPosition());
    target.draw(stairSprite);

    auto stairTargetText = std::make_unique<sf::Text>("..", GameAssets::Get().gameFont);
    stairTargetText->setScale(0.15f, 0.15f);
    stairTargetText->setColor(sf::Color(255, 165, 0));

    auto area = GetAssignedArea();
    if (area) {
        auto textPos = sf::Vector2f(GetCenterPosition().x, GetPosition().y) - sf::Vector2f(
            0.5f * stairTargetText->getGlobalBounds().width,
            6.0f
            );
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

                // TODO player spawn at corrisponding stairs in new level
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
        auto stairTargetText = std::make_unique<sf::Text>(destinationFsNodeName_, GameAssets::Get().gameFont);
        stairTargetText->setScale(0.15f, 0.15f);
        stairTargetText->setColor(sf::Color(255, 255, 0));

        auto area = GetAssignedArea();
        if (area) {
            auto textPos = sf::Vector2f(GetCenterPosition().x, GetPosition().y) - sf::Vector2f(
                0.5f * stairTargetText->getGlobalBounds().width,
                6.0f
                );
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

                // TODO player spawn at root stairs in new level
                Game::Get().SetLevelChange(newFsNodePath);
            }
        }
    }
}