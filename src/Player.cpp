#include "Player.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp> // TODO Probably for debug

#include "World.h"
#include "Game.h"
#include "Helper.h"
#include "IPlayerUsable.h"
#include "Stairs.h"


PlayerDefaultStartEntity::PlayerDefaultStartEntity()
{
}


PlayerDefaultStartEntity::~PlayerDefaultStartEntity()
{
}


PlayerEntity::PlayerEntity() :
AliveEntity(),
useRange_(25.0f),
targettedUsableEnt_(InvalidId)
{
    SetSize(sf::Vector2f(12.0f, 12.0f));
}


PlayerEntity::~PlayerEntity()
{
}


bool PlayerEntity::SetPositionToDefaultStart()
{
    auto area = GetAssignedArea();

    if (!area) {
        return false;
    }

    auto startEnts = area->GetAllEntitiesOfType<PlayerDefaultStartEntity>();
    if (startEnts.size() <= 0) {
        return false;
    }

    auto chosenStartEnt = area->GetEntity<PlayerDefaultStartEntity>(
        startEnts[Helper::GenerateRandomInt<std::size_t>(0, startEnts.size() - 1)]);

    if (!chosenStartEnt) {
        return false;
    }

    SetPosition(chosenStartEnt->GetPosition());
    return true;
}


void PlayerEntity::HandleMovement()
{
    if (!GetAssignedArea()) {
        return;
    }

    sf::Vector2f moveDir;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        moveDir.y -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        moveDir.y += 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        moveDir.x -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        moveDir.x += 1.0f;
    }

    auto d = moveDir * GetStats().GetMoveSpeed() * Game::FrameTimeStep.asSeconds();
    MoveWithCollision(d);
}


void PlayerEntity::HandleUseNearbyObjects()
{
    auto area = GetAssignedArea();

    if (!area) {
        return;
    }

    // use target from last tick if use key was pressed
    if (Game::Get().IsKeyPressedFromEvent(sf::Keyboard::E) && targettedUsableEnt_ != InvalidId) {
        auto usableEnt = dynamic_cast<IPlayerUsable*>(area->GetEntity(targettedUsableEnt_));

        if (usableEnt && usableEnt->IsUsable()) {
            usableEnt->Use(GetAssignedId());
        }
    }

    // update target
    auto entsInRange = area->GetWorldEntitiesInRange(GetCenterPosition(), useRange_);

    EntityId closestUsableEntId = InvalidId;
    float closestUsableEntDistSq = useRange_ * useRange_ + 1.0f;

    for (auto& eInfo : entsInRange) {
        auto usableEnt = dynamic_cast<IPlayerUsable*>(area->GetEntity<WorldEntity>(eInfo.first));

        if (usableEnt && usableEnt->IsUsable() && eInfo.second < closestUsableEntDistSq) {
            closestUsableEntId = eInfo.first;
            closestUsableEntDistSq = eInfo.second;
        }
    }

    targettedUsableEnt_ = closestUsableEntId;
}


void PlayerEntity::Tick()
{
    HandleMovement();
    HandleUseNearbyObjects();
}


void PlayerEntity::Render(sf::RenderTarget& target)
{
    sf::Sprite playerSprite(GameAssets::Get().playerSpriteSheet, sf::IntRect(0, 0, 16, 16));
    playerSprite.setPosition(GetCenterPosition() - sf::Vector2f(8.0f, 8.0f));
    target.draw(playerSprite);
}
