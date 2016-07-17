#include "Player.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/RectangleShape.hpp> // TODO debug

#include "World.h"
#include "Game.h"


PlayerEntity::PlayerEntity() :
AliveEntity()
{
    SetSize(BaseTile::TileSize * 1.00f); // TODO debug?
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

    auto d = moveDir * GetStats().GetMoveSpeed(); // TODO dt

    std::unique_ptr<sf::Shape> r = std::make_unique<sf::RectangleShape>(GetSize());
    r->setPosition(GetPosition());
    r->setFillColor(sf::Color::Magenta);
    GetAssignedArea()->AddDebugShape(Game::FrameTimeStep, r);

    MoveWithCollision(d);

    // TODO test
    //BaseTile* collidedTile = nullptr;
    //u32 collidedTileX, collidedTileY;
    //sf::Vector2f collisionNormal;

    //auto collisionTime = GetAssignedArea()->CheckEntRectTileSweepCollision(
    //    CollisionRectInfo(GetRectangle(), sf::Vector2f(d.x, 0.0f)),
    //    &collidedTile, &collidedTileX, &collidedTileY, &collisionNormal
    //    );

    //d.x *= std::max(0.0f, collisionTime - 0.01f);
    //SetPosition(GetPosition() + sf::Vector2f(d.x, 0.0f));

    //collisionTime = GetAssignedArea()->CheckEntRectTileSweepCollision(
    //    CollisionRectInfo(GetRectangle(), sf::Vector2f(0.0f, d.y)),
    //    &collidedTile, &collidedTileX, &collidedTileY, &collisionNormal
    //    );

    //d.y *= std::max(0.0f, collisionTime - 0.01f);
    //SetPosition(GetPosition() + sf::Vector2f(0.0f, d.y));
}


void PlayerEntity::Render(sf::RenderTarget& target)
{
    // TODO
    //sf::RectangleShape r;
    //r.setPosition(GetPosition());
    //r.setSize(GetSize());
    //r.setFillColor(sf::Color::Magenta);
    //target.draw(r);
}
