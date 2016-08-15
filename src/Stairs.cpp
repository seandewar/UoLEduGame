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


bool StairEntity::IsAvailable() const
{
    switch (Game::Get().GetDirector().GetCurrentObjectiveType()) {
    case GameObjectiveType::BossFight:
    case GameObjectiveType::Complete:
    case GameObjectiveType::End:
        return false;

    default:
        return true;
    }
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
    sf::Sprite stairSprite(GameAssets::Get().stairsSpriteSheet);

    if (!IsAvailable()) {
        stairSprite.setTextureRect(sf::IntRect(16, 16, 16, 16));
    }
    else {
        stairSprite.setTextureRect(sf::IntRect(16, 0, 16, 16));
    }

    stairSprite.setPosition(GetPosition());
    target.draw(stairSprite);

    auto area = GetAssignedArea();

    if (area) {
        auto stairTargetText = std::make_unique<sf::Text>("..", GameAssets::Get().gameFont, 18);
        stairTargetText->setScale(Game::Get().IsInMapMode() ? sf::Vector2f(0.6f, 0.6f) : sf::Vector2f(0.15f, 0.15f));

        if (!IsAvailable()) {
            stairTargetText->setFillColor(sf::Color(150, 150, 150, Game::Get().IsInMapMode() ? 255 : 150));
        }
        else {
            stairTargetText->setFillColor(sf::Color(255, 255, 0));
        }

        auto textPos = sf::Vector2f(GetCenterPosition().x, GetPosition().y) -
            sf::Vector2f(0.5f * stairTargetText->getGlobalBounds().width, 4.0f);

        stairTargetText->setPosition(textPos);

        sf::Color shadowColor;

        if (!IsAvailable()) {
            shadowColor = sf::Color(0, 0, 0, 150);
        }
        else {
            shadowColor = sf::Color(0, 0, 0, 255);
        }

        area->AddFrameUIRenderable(Helper::GetTextDropShadow(*stairTargetText, 
            Game::Get().IsInMapMode() ? sf::Vector2f(2.0f, 2.0f) : sf::Vector2f(0.5f, 0.5f),
            shadowColor));
        area->AddFrameUIRenderable(std::move(stairTargetText));
    }
}


bool UpStairEntity::IsUsable(EntityId playerId) const
{
    return IsAvailable();
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
    sf::Sprite stairSprite(GameAssets::Get().stairsSpriteSheet);

    if (!IsAvailable()) {
        stairSprite.setTextureRect(sf::IntRect(0, 16, 16, 16));
    }
    else {
        stairSprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
    }

    stairSprite.setPosition(GetPosition());
    target.draw(stairSprite);

    if (!destinationFsNodeName_.empty()) {
        auto area = GetAssignedArea();

        if (area) {
            auto stairTargetText = std::make_unique<sf::Text>(destinationFsNodeName_, GameAssets::Get().gameFont, 18);
            stairTargetText->setScale(Game::Get().IsInMapMode() ? sf::Vector2f(0.8f, 0.8f) : sf::Vector2f(0.15f, 0.15f));

            if (!IsAvailable()) {
                stairTargetText->setFillColor(sf::Color(150, 150, 150, Game::Get().IsInMapMode() ? 255 : 150));
            }
            else {
                stairTargetText->setFillColor(sf::Color(255, 255, 0));
            }

            auto textPos = sf::Vector2f(GetCenterPosition().x, GetPosition().y) -
                sf::Vector2f(0.5f * stairTargetText->getGlobalBounds().width, 4.0f);

            stairTargetText->setPosition(textPos);

            sf::Color shadowColor;

            if (!IsAvailable()) {
                shadowColor = sf::Color(0, 0, 0, 150);
            }
            else {
                shadowColor = sf::Color(0, 0, 0, 255);
            }

            area->AddFrameUIRenderable(Helper::GetTextDropShadow(*stairTargetText,
                Game::Get().IsInMapMode() ? sf::Vector2f(3.0f, 3.0f) : sf::Vector2f(0.5f, 0.5f),
                shadowColor));
            area->AddFrameUIRenderable(std::move(stairTargetText));
        }
    }
}


bool DownStairEntity::IsUsable(EntityId playerId) const
{
    return IsAvailable() && !destinationFsNodeName_.empty();
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