#include "Player.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/RectangleShape.hpp> // TODO debug

#include "World.h"


PlayerEntity::PlayerEntity() :
AliveEntity()
{
    SetSize(BaseTile::TileSize * 0.9f); // TODO debug?
}


PlayerEntity::~PlayerEntity()
{
}


void PlayerEntity::Tick()
{
    sf::Vector2f moveDir;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        moveDir.y -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        moveDir.y += 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        moveDir.x -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        moveDir.x += 1.0f;
    }

    auto velocity = moveDir * GetStats().GetMoveSpeed();

    // TODO test
    BaseTile* collidedTile = nullptr;
    u32 collidedTileX, collidedTileY;
    sf::Vector2f collisionNormal;

    auto collisionTime = GetAssignedArea()->CheckEntRectTileSweepCollision(
        CollisionRectInfo(GetRectangle(), sf::Vector2f(velocity.x, 0.0f)),
        &collidedTile, &collidedTileX, &collidedTileY, &collisionNormal
        );

    velocity.x *= std::max(0.0f, collisionTime - 0.01f);
    SetPosition(GetPosition() + sf::Vector2f(velocity.x, 0.0f));

    collisionTime = GetAssignedArea()->CheckEntRectTileSweepCollision(
        CollisionRectInfo(GetRectangle(), sf::Vector2f(0.0f, velocity.y)),
        &collidedTile, &collidedTileX, &collidedTileY, &collisionNormal
        );

    velocity.y *= std::max(0.0f, collisionTime - 0.01f);
    SetPosition(GetPosition() + sf::Vector2f(0.0f, velocity.y));
}


void PlayerEntity::Render(sf::RenderTarget& target)
{
    // TODO
    sf::RectangleShape r;
    r.setPosition(GetPosition());
    r.setSize(GetSize());
    r.setFillColor(sf::Color::Magenta);
    target.draw(r);
}
